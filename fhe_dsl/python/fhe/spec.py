#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
CompileSpec: Unified FHE compilation input descriptor.

.. deprecated::
    Use ModelSpec and FuncSpec from ace.fhe.config.spec instead.
    CompileSpec, ModelEntity, FuncEntity, CompileConfig, InputSpec,
    RuntimeConfig, and DatasetSource are all replaced by the new
    ModelSpec/FuncSpec system. This module will be removed in a future release.

Three-layer architecture:

Three-layer architecture:
- Layer 1: Entity (ModelEntity / FuncEntity) - describes WHAT to compile
- Layer 2: CompileConfig - describes HOW to compile
- Layer 3: RuntimeConfig - describes HOW to run/validate

Usage:
    from ace.fhe.spec import CompileSpec, ModelEntity, CompileConfig, RuntimeConfig

    # For models
    spec = CompileSpec(
        entity=ModelEntity(
            name="resnet20",
            model_class=ResNet_CIFAR,
            model_init_args=(BasicBlock, [3, 3, 3]),
            model_init_kwargs={"num_classes": 10},
            model_post_init=load_resnet20_pretrained,
            weights_required=True,
        ),
        compile=CompileConfig(
            input_spec=[InputSpec(shape=(1, 3, 32, 32), dtype=torch.float32)],
            compile_options={
                "vec": {"conv_parl": True, "ssf": True},
                "ckks": {"q0": 60, "sf": 56, "N": 65536, "icl": 17},
            },
            encrypt_inputs=["x"],
            library="phantom",
            device="cuda",
        ),
        runtime=RuntimeConfig(
            input_source=DatasetSource(dataset="cifar10", num_samples=1),
            validation=verify_accuracy,
        ),
    )

    # For functions
    spec = CompileSpec(
        entity=FuncEntity(
            name="add_func",
            func=add_func,
        ),
        compile=CompileConfig(
            input_spec=[
                InputSpec(shape=(1, 10), dtype=torch.float32),
                InputSpec(shape=(1, 10), dtype=torch.float32),
            ],
            encrypt_inputs=["x", "y"],
        ),
    )
"""
import warnings
from dataclasses import dataclass, field
from typing import Any, Callable, Dict, List, Optional, Union

warnings.warn(
    "ace.fhe.spec is deprecated. Use ace.fhe.config.spec.ModelSpec and "
    "ace.fhe.config.spec.FuncSpec instead. This module will be removed in a "
    "future release.",
    DeprecationWarning,
    stacklevel=2,
)
import torch


# =============================================================================
# Layer 1: Entity (实体描述)
# =============================================================================

@dataclass
class ModelEntity:
    """Model entity for FHE compilation."""
    name: str

    # Model construction
    model_class: type  # PyTorch nn.Module subclass
    model_init_args: tuple = ()
    model_init_kwargs: dict = field(default_factory=dict)

    # Post-initialization (e.g., loading weights)
    model_post_init: Optional[Callable] = None

    # Resource requirements
    weights_required: bool = False
    constants_required: bool = False

    # IR structure validation (optional)
    ir_ops: Optional[List[str]] = None


@dataclass
class FuncEntity:
    """Function entity for FHE compilation."""
    name: str
    func: Callable  # Python function

    # IR structure validation (optional)
    ir_ops: Optional[List[str]] = None


# =============================================================================
# Layer 2: CompileConfig (编译配置)
# =============================================================================

@dataclass
class InputSpec:
    """Describes input shape and dtype (compilation only needs this)."""
    shape: tuple
    dtype: torch.dtype = torch.float32


@dataclass
class CompileConfig:
    """Configuration for FHE compilation."""
    # Input specification
    input_spec: List[InputSpec]

    # Compilation options
    compile_options: Dict[str, Any] = field(default_factory=dict)
    encrypt_inputs: List[str] = field(default_factory=list)

    # Compilation target
    frontend: str = "torch"
    library: str = "antlib"
    device: str = "cpu"

    # Additional compilation options
    output_format: str = "so"
    optimization_level: str = "O2"


# =============================================================================
# Layer 3: RuntimeConfig (运行时配置)
# =============================================================================

@dataclass
class DatasetSource:
    """Runtime data source."""
    dataset: str = None  # "cifar10", "cifar100"
    num_samples: int = 1
    offset: int = 0


@dataclass
class RuntimeConfig:
    """Runtime configuration for execution and validation."""
    # Input data source
    input_source: Optional[DatasetSource] = None

    # Validation
    validation: Optional[Callable] = None  # validation function
    metric: Optional[Callable] = None       # evaluation metric

    # Pre/Post processing
    preprocessing: dict = field(default_factory=dict)
    postprocessing: Optional[Callable] = None


# =============================================================================
# Combined: CompileSpec
# =============================================================================

@dataclass
class CompileSpec:
    """Unified FHE compilation specification with three-layer architecture."""
    entity: Union[ModelEntity, FuncEntity]
    compile: CompileConfig
    runtime: Optional[RuntimeConfig] = None

    # Convenience property for backward compatibility
    @property
    def name(self) -> str:
        return self.entity.name

    def create(self):
        """Create the compilable entity (model or function)."""
        if isinstance(self.entity, ModelEntity):
            model = self.entity.model_class(
                *self.entity.model_init_args,
                **self.entity.model_init_kwargs
            )
            if self.entity.model_post_init is not None:
                self.entity.model_post_init(model)
            model._fhe_name = self.entity.name
            return model
        elif isinstance(self.entity, FuncEntity):
            return self.entity.func
        else:
            raise ValueError(f"Unknown entity type: {type(self.entity)}")

    @classmethod
    def from_example_inputs(cls, entity: Union[ModelEntity, FuncEntity],
                            example_inputs: tuple,
                            compile_options: Dict = None,
                            encrypt_inputs: List[str] = None,
                            **kwargs):
        """Convenience constructor from example_inputs tuple."""
        input_spec = []
        for inp in example_inputs:
            if hasattr(inp, 'shape') and hasattr(inp, 'dtype'):
                input_spec.append(InputSpec(shape=inp.shape, dtype=inp.dtype))
            else:
                raise ValueError(f"Expected tensor, got {type(inp)}")

        compile = CompileConfig(
            input_spec=input_spec,
            compile_options=compile_options or {},
            encrypt_inputs=encrypt_inputs or [],
            **{k: v for k, v in kwargs.items() if k in ['frontend', 'library', 'device']}
        )

        return cls(entity=entity, compile=compile)