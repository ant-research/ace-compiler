"""Makefile pipeline: compare declarations and numerical values directly."""
import fcntl
import json
import math
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import tempfile

FORMAT = 2


def quote(value):
    return json.dumps(str(value), ensure_ascii=False)


def safe_name(name):
    if not name or any(ord(c) < 32 or ord(c) == 127 for c in name):
        raise ValueError('invalid object name')


def values(path, count):
    raw = path.read_bytes()
    data = [float(x) for x in raw.split()]
    if len(data) != count or any(not math.isfinite(x) for x in data):
        raise ValueError(f'invalid numerical data: {path}')
    return raw, data


def load_export(work):
    root = work / 'export'
    lines = (root / 'encode.inc').read_text().splitlines()
    if not lines or lines[0] != 'CLIB_EXPORT 2':
        raise ValueError('unsupported encode.inc version; run make export')
    result = dict(root=root, objects={}, outputs={}, rotations=[])
    for line in lines[1:]:
        r = shlex.split(line)
        if not r:
            raise ValueError('empty export record')
        if r[0] == 'CONFIG' and len(r) == 3:
            result['N'], result['Q'] = map(int, r[1:])
        elif r[0] == 'EXPORT' and len(r) == 2:
            result['identity'] = r[1]
        elif r[0] == 'ROTATION' and len(r) == 2:
            result['rotations'].append(int(r[1]))
        elif r[0] == 'OBJECT' and len(r) == 7:
            _, kind, name, q, degree, count, path = r
            safe_name(name)
            if kind not in ('plain', 'input') or degree != '1':
                raise ValueError('invalid declaration type/degree')
            key = kind + ':' + name
            if key in result['objects']:
                raise ValueError('duplicate object: ' + key)
            result['objects'][key] = dict(kind=kind, name=name, q=int(q), count=int(count), path=path)
        elif r[0] == 'OUTPUT' and len(r) == 7:
            _, name, q, degree, count, path, tolerance = r
            safe_name(name)
            if Path(name).is_absolute() or any(x in ('.', '..') for x in name.split('/')):
                raise ValueError('invalid output path: ' + name)
            if name in result['outputs']:
                raise ValueError('duplicate output: ' + name)
            result['outputs'][name] = dict(name=name, q=int(q), degree=int(degree), count=int(count), path=path, tolerance=float(tolerance))
        else:
            raise ValueError('invalid encode.inc record: ' + line[:120])
    if result.get('N') not in (1024, 8192, 65536) or result.get('Q', 0) not in range(32, 63, 2) or len(result.get('identity', '')) != 32:
        raise ValueError('invalid export configuration')
    for obj in result['objects'].values():
        if obj['count'] != result['N'] // 2:
            raise ValueError('declaration requires N/2 slots')
    for obj in result['outputs'].values():
        if obj['count'] not in (0, result['N'] // 2) or obj['degree'] not in (1, 2) or not math.isfinite(obj['tolerance']) or obj['tolerance'] < 0:
            raise ValueError('invalid output declaration')
    for record in [*result['objects'].values(), *result['outputs'].values()]:
        p = (root / record['path']).resolve()
        if not p.is_relative_to(root.resolve()):
            raise ValueError('export data path escapes its directory')
        values(p, record['count'])
        if record['q'] < 2 or record['q'] > result['Q'] or record['q'] % 2:
            raise ValueError('invalid Q: ' + record['name'])
    return result


def run(command):
    subprocess.run(list(map(str, command)), check=True,
                   env={**os.environ, 'OMP_WAIT_POLICY': 'PASSIVE', 'OMP_NUM_THREADS': '32'})


def atomic_text(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    temp = path.with_name(path.name + '.pending')
    temp.write_text(text)
    temp.replace(path)


def remove_directory(path):
    if path.is_symlink():
        path.unlink()
    elif path.exists():
        shutil.rmtree(path)


def replace_directory(staging, target):
    previous = target.with_name(target.name + '.previous')
    remove_directory(previous)
    if target.exists():
        target.rename(previous)
    try:
        staging.rename(target)
    except BaseException:
        if previous.exists():
            previous.rename(target)
        raise
    remove_directory(previous)


def check_context(root):
    summary = json.loads((root / 'case.json').read_text())
    expected = f'CLIBIDENT2 {summary["profile_id"]} {summary["keyset_id"]}\n'
    for folder in ('evaluation', 'private'):
        if (root / folder / 'identity.txt').read_text() != expected:
            raise ValueError('context identity mismatch')
    for name in ('param.txt', 'bootstrap.txt', 'scales.txt', 'plain/u0_sdk.txt', 'plain/u0hat_sdk.txt'):
        if not (root / 'evaluation' / name).is_file():
            raise ValueError('missing context file: ' + name)
    return summary


def configuration(export):
    return dict(N=export['N'], q_limbs=export['Q'], rotations=sorted(set(export['rotations'])))


def prepare_context(sdk, work):
    export = load_export(work)
    config = configuration(export)
    requested = dict(config=config, format=FORMAT, build=(sdk / 'tools/build.id').read_text().strip())
    root = work / 'context'
    with (work / '.context.lock').open('a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        if (root / 'configuration.json').exists() and json.loads((root / 'configuration.json').read_text()) == requested:
            summary = check_context(root)
            if any(summary[k] != v for k, v in config.items()):
                raise ValueError('cached context configuration mismatch')
            print('ANT context: reuse', flush=True)
            return
        with tempfile.TemporaryDirectory(prefix='.context-', dir=work) as temp:
            staging = Path(temp)
            config_file = staging / 'config.json'
            config_file.write_text(json.dumps(config))
            run([sdk / 'tools/clib-ant-tool', 'prepare', '--config', config_file, '--out', staging / 'case'])
            summary = check_context(staging / 'case')
            if summary['bootstrap_max_q_limbs'] != export['Q'] - 30:
                raise ValueError('ANT bootstrap profile differs from export')
            (staging / 'case/configuration.json').write_text(json.dumps(requested))
            replace_directory(staging / 'case', root)


def read_receipt(work, export):
    root = work / 'encoded'
    for name in ('encode.inc', 'trace.txt'):
        if (root / name).read_bytes() != (export['root'] / name).read_bytes():
            raise ValueError('encoding is stale; run make encode')
    for obj in export['objects'].values():
        if (root / obj['path']).read_bytes() != (export['root'] / obj['path']).read_bytes():
            raise ValueError('encoded values differ from export: ' + obj['name'])
    rows = [shlex.split(s) for s in (root / 'encoded.inc').read_text().splitlines()]
    if rows[:2] != [['CLIB_ENCODED', '2'], ['EXPORT', export['identity']]]:
        raise ValueError('encoding is stale; run make encode')
    objects = {}
    material = profile = keyset = None
    for r in rows[2:]:
        if r[0] == 'MATERIAL' and len(r) == 2:
            material = Path(r[1])
        elif r[0] == 'IDENTITY' and len(r) == 3:
            profile, keyset = r[1:]
        elif r[0] == 'OBJECT' and len(r) == 4:
            _, kind, name, path = r
            if kind + ':' + name in objects or not Path(path).is_file():
                raise ValueError('missing or duplicate encoded object')
            objects[kind + ':' + name] = path
        else:
            raise ValueError('invalid encoding receipt')
    if material is None or material.resolve() != (work / 'context/evaluation').resolve() or set(objects) != set(export['objects']):
        raise ValueError('encoded context or object set mismatch')
    if (material / 'identity.txt').read_text() != f'CLIBIDENT2 {profile} {keyset}\n':
        raise ValueError('encoded context identity mismatch')
    return material.parent


def encoding_matches(target, export, summary):
    description = dict(objects=export['objects'], profile=summary['profile_id'], keyset=summary['keyset_id'], format=FORMAT)
    path = target / 'objects.json'
    if not path.exists() or json.loads(path.read_text()) != description:
        return False
    for index, obj in enumerate(export['objects'].values()):
        raw = target / obj['path']
        encoded = target / (str(index) + ('.pt' if obj['kind'] == 'plain' else '.ct'))
        if not raw.exists() or raw.read_bytes() != (export['root'] / obj['path']).read_bytes() or not encoded.is_file():
            return False
    return True


def write_receipt(target, final, export, context, summary):
    records = ['CLIB_ENCODED 2', 'EXPORT ' + export['identity'], 'MATERIAL ' + quote(context / 'evaluation'), 'IDENTITY ' + summary['profile_id'] + ' ' + summary['keyset_id']]
    for index, obj in enumerate(export['objects'].values()):
        filename = str(index) + ('.pt' if obj['kind'] == 'plain' else '.ct')
        records.append(f'OBJECT {obj["kind"]} {quote(obj["name"])} {quote(final / filename)}')
    for name in ('encode.inc', 'trace.txt'):
        shutil.copy2(export['root'] / name, target / name)
    atomic_text(target / 'encoded.inc', '\n'.join(records) + '\n')


def encode(sdk, work):
    export = load_export(work)
    context = work / 'context'
    summary = check_context(context)
    if any(summary[k] != v for k, v in configuration(export).items()):
        raise ValueError('context differs from export; run make context')
    target = work / 'encoded'
    with (work / '.encode.lock').open('a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        if encoding_matches(target, export, summary):
            write_receipt(target, target, export, context, summary)
            print('ANT encoded objects: reuse', flush=True)
            return
        with tempfile.TemporaryDirectory(prefix='.encode-', dir=work) as temp:
            staging = Path(temp) / 'encoded'
            staging.mkdir()
            requests = []
            for index, obj in enumerate(export['objects'].values()):
                filename = str(index) + ('.pt' if obj['kind'] == 'plain' else '.ct')
                requests.append(f'{"encode" if obj["kind"] == "plain" else "encrypt"} {obj["q"]} {quote(export["root"] / obj["path"])} {quote(staging / filename)}\n')
                raw = staging / obj['path']
                raw.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(export['root'] / obj['path'], raw)
            req = Path(temp) / 'requests.txt'
            req.write_text(''.join(requests))
            run([sdk / 'tools/clib-ant-tool', 'batch', '--private', context / 'private', '--requests', req])
            (staging / 'objects.json').write_text(json.dumps(dict(objects=export['objects'], profile=summary['profile_id'], keyset=summary['keyset_id'], format=FORMAT)))
            write_receipt(staging, target, export, context, summary)
            replace_directory(staging, target)
        print(f'ANT encoded objects: {len(export["objects"])}', flush=True)


def verify(sdk, work, output):
    export = load_export(work)
    if (output / 'run.ok').read_text() != export['identity'] + '\n':
        raise ValueError('no successful run for this export; run make run')
    context = read_receipt(work, export)
    report = {}
    for name, obj in export['outputs'].items():
        ct = output / (name + '.ct')
        decoded = output / (name + '.json')
        run([sdk / 'tools/clib-ant-tool', 'decrypt', '--private', context / 'private', '--input', ct, '--out', decoded])
        actual = json.loads(decoded.read_text())
        if len(actual) != export['N'] // 2 or any(not math.isfinite(x) for x in actual):
            raise ValueError('invalid decoded output: ' + name)
        item = dict(slots=len(actual), q_limbs=obj['q'])
        if obj['count']:
            _, expected = values(export['root'] / obj['path'], obj['count'])
            error = max(abs(x - y) for x, y in zip(actual, expected))
            item.update(max_abs_error=error, tolerance=obj['tolerance'], passed=error <= obj['tolerance'])
            print(f'{"PASS" if item["passed"] else "FAIL"} {name}: {len(actual)} slots, max error {error:.6g}', flush=True)
        else:
            item.update(passed=True, comparison='not requested')
            print(f'DECODED {name}: {len(actual)} slots (no reference)', flush=True)
        report[name] = item
    atomic_text(output / 'verification.json', json.dumps(report, indent=2) + '\n')
    if not all(item['passed'] for item in report.values()):
        raise ValueError('output verification failed')
    print('Results: ' + str(output), flush=True)


def main(command, sdk, work, output=None):
    work = Path(work).resolve()
    if command == 'application-context':
        prepare_context(sdk, work)
    elif command == 'application-encode':
        encode(sdk, work)
    elif command == 'application-verify':
        verify(sdk, work, Path(output).resolve())
