# fhe_dsl/python/fhe/ir/export/serializer.py

import pickle
from pathlib import Path
from typing import Any

def _safe_serialize(obj):
    """Safely serialize functions - for debugging purposes"""
    try:
        # Try direct serialization
        pickle.dumps(obj)
        return obj
    except Exception as e:
        # If that fails, a secure copy is created
        if hasattr(obj, '__dict__'):
            safe_obj = type(obj).__new__(type(obj))
            safe_obj.__dict__ = {}
            for key, value in obj.__dict__.items():
                try:
                    pickle.dumps(value)
                    safe_obj.__dict__[key] = value
                except:
                    safe_obj.__dict__[key] = f"<unserializable:{key}:{type(value).__name__}>"
            return safe_obj
        else:
            return f"<unserializable object: {type(obj).__name__}>"

class IRSerializer:
    @staticmethod
    def save(ir_graph, filename, force_pickle: bool = False):
        filepath = Path(filename)
        # Only change extension to .pkl if explicitly requested or if no extension
        if force_pickle or (filepath.suffix == '' or filepath.suffix == '.air'):
            filepath = filepath.with_suffix('.pkl')

        # Safety check before saving (optional for debugging)
        safe_graph = _safe_serialize(ir_graph)

        with open(filepath, 'wb') as f:
            pickle.dump(safe_graph, f)  # IRNode's __getstate__ is handled automatically
    
    @staticmethod
    def load(filename):
        filepath = Path(filename)
        if filepath.suffix != '.pkl':
            filepath = filepath.with_suffix('.pkl')
        with open(filepath, 'rb') as f:
            return pickle.load(f)
