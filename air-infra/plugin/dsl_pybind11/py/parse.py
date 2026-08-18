import ast
import air_dsl as dsl

tran = dsl.DSL()

# Parse Python code to generate AST
def ast_parse(pyfile):
    with open(pyfile, 'r') as file:
        code = file.read()

    return ast.parse(code)

def BinOp(value):
    if value == 'Add()' :
        tran.add(2, 3)
    elif value == 'Sub()' :
        tran.sub(2, 3)
    elif value == 'Mul()' :
        tran.mul(2, 3)
    else :
        print('Err: Unknow Operator')


# Traverse and access each node of the AST
def visit_node(node, indent=0):
    # Print the node type
    print('  ' * indent + f'{node.__class__.__name__})') # (lineno={node.lineno})')

    name = node.__class__.__name__

    if name == 'Module': 
        pass
    elif name == 'Name' :
        pass
    elif name == 'Assign' :
        tran.Assign()
    elif name == 'BinOp' :
        BinOp("Add()")
    elif name == 'Constant' :
        print("TODO:")
    elif name == 'Store' :
        print("TODO:")
    else :
        print('Err: Unknow AST statement')

    # Recursively traverses the child node
    for child in ast.iter_child_nodes(node):
        visit_node(child, indent + 1)