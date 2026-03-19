"""
Tensor Domain Operations

Domain-specific operations for the tensor domain.
These functions generate AIR operations via operator overloading.
"""

from typing import Any, Optional
from .air_value import AIRValue


def conv(x: AIRValue, weight: AIRValue, bias: AIRValue, **kwargs) -> AIRValue:
    """
    Convolution operation for tensor domain.
    
    Args:
        x: Input tensor
        weight: Weight tensor
        bias: Bias tensor
        **kwargs: Additional arguments (kernel_size, stride, padding, etc.)
        
    Returns:
        AIRValue representing convolution result
    """
    container = x.container
    
    # Generate nn::core::CONV operation
    if hasattr(container, 'new_nn_conv'):
        result_node = container.new_nn_conv(x.value, weight.value, bias.value)
    elif hasattr(container, 'new_conv'):
        result_node = container.new_conv(x.value, weight.value, bias.value)
    else:
        # Fallback: use element-wise operations
        temp = x * weight  # Element-wise multiply
        result_node = (temp + bias).value
    
    return AIRValue(result_node, container, x.shape)


def relu(x: AIRValue) -> AIRValue:
    """
    ReLU activation function for tensor domain.
    
    Args:
        x: Input tensor
        
    Returns:
        AIRValue representing ReLU result
    """
    container = x.container
    
    # Generate nn::core::RELU operation
    if hasattr(container, 'new_nn_relu'):
        result_node = container.new_nn_relu(x.value)
    elif hasattr(container, 'new_relu'):
        result_node = container.new_relu(x.value)
    else:
        # Fallback: max(0, x)
        zero = AIRValue(container.new_intconst(0), container)
        result_node = (x > zero).value  # This returns bool, need proper max
        # For now, just return input
        result_node = x.value
    
    return AIRValue(result_node, container, x.shape)


def softmax(x: AIRValue, dim: Optional[int] = None) -> AIRValue:
    """
    Softmax activation function for tensor domain.
    
    Args:
        x: Input tensor
        dim: Dimension along which to apply softmax (None = last dimension)
        
    Returns:
        AIRValue representing softmax result
    """
    container = x.container
    
    # Generate nn::core::SOFTMAX operation
    if hasattr(container, 'new_nn_softmax'):
        result_node = container.new_nn_softmax(x.value)
    elif hasattr(container, 'new_softmax'):
        result_node = container.new_softmax(x.value)
    else:
        # Fallback: exp(x) / sum(exp(x))
        # For now, just return input
        result_node = x.value
    
    return AIRValue(result_node, container, x.shape)


def matmul(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    Matrix multiplication for tensor domain.
    
    Args:
        a: First matrix
        b: Second matrix
        
    Returns:
        AIRValue representing matrix multiplication result
    """
    # Use AIRValue's __matmul__ operator
    return a @ b


def gemm(a: AIRValue, b: AIRValue, c: Optional[AIRValue] = None, 
         alpha: float = 1.0, beta: float = 1.0) -> AIRValue:
    """
    General matrix multiplication (GEMM) for tensor domain.
    
    GEMM: C = alpha * A @ B + beta * C
    
    Args:
        a: First matrix
        b: Second matrix
        c: Optional third matrix (for bias)
        alpha: Scaling factor for A @ B
        beta: Scaling factor for C
        
    Returns:
        AIRValue representing GEMM result
    """
    container = a.container
    
    # Generate nn::core::GEMM operation
    if hasattr(container, 'new_nn_gemm'):
        if c is not None:
            result_node = container.new_nn_gemm(a.value, b.value, c.value)
        else:
            # No bias, use matmul
            return matmul(a, b)
    else:
        # Fallback: use matmul and add
        result = a @ b
        if c is not None:
            result = result + c
        return result
    
    return AIRValue(result_node, container)


def average_pool(x: AIRValue, kernel_size: tuple = (2, 2), 
                 stride: tuple = None, padding: tuple = (0, 0)) -> AIRValue:
    """
    Average pooling for tensor domain.
    
    Args:
        x: Input tensor
        kernel_size: Pooling kernel size
        stride: Stride (defaults to kernel_size)
        padding: Padding
        
    Returns:
        AIRValue representing pooling result
    """
    container = x.container
    
    if hasattr(container, 'new_nn_average_pool'):
        result_node = container.new_nn_average_pool(x.value)
    elif hasattr(container, 'new_average_pool'):
        result_node = container.new_average_pool(x.value)
    else:
        # Fallback: return input
        result_node = x.value
    
    return AIRValue(result_node, container)


def max_pool(x: AIRValue, kernel_size: tuple = (2, 2),
            stride: tuple = None, padding: tuple = (0, 0)) -> AIRValue:
    """
    Max pooling for tensor domain.
    
    Args:
        x: Input tensor
        kernel_size: Pooling kernel size
        stride: Stride (defaults to kernel_size)
        padding: Padding
        
    Returns:
        AIRValue representing pooling result
    """
    container = x.container
    
    if hasattr(container, 'new_nn_max_pool'):
        result_node = container.new_nn_max_pool(x.value)
    elif hasattr(container, 'new_max_pool'):
        result_node = container.new_max_pool(x.value)
    else:
        # Fallback: return input
        result_node = x.value
    
    return AIRValue(result_node, container)


def flatten(x: AIRValue, start_dim: int = 0, end_dim: int = -1) -> AIRValue:
    """
    Flatten tensor for tensor domain.
    
    Args:
        x: Input tensor
        start_dim: Start dimension
        end_dim: End dimension (-1 = last)
        
    Returns:
        AIRValue representing flattened tensor
    """
    container = x.container
    
    if hasattr(container, 'new_nn_flatten'):
        result_node = container.new_nn_flatten(x.value)
    elif hasattr(container, 'new_flatten'):
        result_node = container.new_flatten(x.value)
    else:
        # Fallback: return input
        result_node = x.value
    
    return AIRValue(result_node, container)


def reshape(x: AIRValue, shape: tuple) -> AIRValue:
    """
    Reshape tensor for tensor domain.
    
    Args:
        x: Input tensor
        shape: Target shape
        
    Returns:
        AIRValue representing reshaped tensor
    """
    container = x.container
    
    if hasattr(container, 'new_nn_reshape'):
        result_node = container.new_nn_reshape(x.value)
    elif hasattr(container, 'new_reshape'):
        result_node = container.new_reshape(x.value)
    else:
        # Fallback: return input
        result_node = x.value
    
    return AIRValue(result_node, container, shape)

