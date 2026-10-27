# How to add new domain operators for the PyDSL

1. Update the file opcode\_def.inc with newly create domain-specific operators. For NN, update the file nn-addon/core/od/opcode\_def.py to add the new operator.

2. Create a domain subdirectory within the src directory. Currently, there are two domains: tensor and vector. The rest of the domains including HPoly and LPoly are to be added.

3. Create files in the newly created domain subdirectory to outline the process of creating AIR operators for new domain-specific operations.

Using Tensor as an example, a class named TENSOR\_API is created. To define a softmax operator, it is added as a method within the TENSOR\_API class. The softmax method includes the implementation details for creating the NN operator for softmax.

4. In the file air-infra/plugin/dsl\_pybind11/src/air\_dsl\_bindings.cxx, implement the PythonBinding API to enable AIR creation to be invoked directly from Python.

Using Tensor as an example, a Python class is created in the form of py::class_<TENSOR_API>(m, "TensorAPI"). Then the softmax method is defined in the form of ".def("softmax", &TENSOR\_API::Softmax, py::return\_value\_policy::reference)" within the Python class.

5. Create a domain-specific python file within the pydsl directory to define a class that maps the newly created Python operator to the corresponding class. Within the class, utilize the PythonBinding API to interact with and operate on the AIR.

Using Tensor as an example, a base Python class, TensorCallMacro(CallMacro) is defined. A subclass Softmax(TensorCallMacro) is then defined. In this new class, three arguments are specified in the argreps method to indicate that Softmax has three operands . The implementation details are provided for parsing these arguments and invoking the PythonBinding API to create a new NN operator.
