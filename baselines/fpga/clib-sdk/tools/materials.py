"""Context identifiers shared by ANT preparation and object writers."""
from pathlib import Path
import uuid


def tag(path, profile_id, key_id):
    path = Path(path)
    header, body = path.read_text().split('\n', 1)
    fields = header.split()
    if fields[0] not in ('CLIBCT1', 'CLIBPT1'):
        raise ValueError('not a clib object')
    path.write_text(' '.join(fields[:5] + [profile_id, key_id if fields[0] == 'CLIBCT1' else '-']) + '\n' + body)


def seal(root):
    root = Path(root)
    identity = root / 'identity.txt'
    if identity.exists():
        magic, profile_id, key_id = identity.read_text().split()
        if magic != 'CLIBIDENT2':
            raise ValueError('old context format; regenerate materials')
        uuid.UUID(profile_id)
        uuid.UUID(key_id)
    else:
        profile_id, key_id = str(uuid.uuid4()), str(uuid.uuid4())
        identity.write_text(f'CLIBIDENT2 {profile_id} {key_id}\n')
    for p in [*root.glob('*.ct'), *root.glob('*.pt')]:
        tag(p, profile_id, key_id)
    return profile_id, key_id
