#!/usr/bin/env python3
"""
Extended interpreter with array, foreach, match, and function call support
"""

import re
import sys

class Interpreter:
    def __init__(self):
        self.variables = {}

    def remove_comments(self, code):
        return re.sub(r'/\*.*?\*/', '', code, flags=re.DOTALL)

    def tokenize(self, code):
        code = self.remove_comments(code)
        lines = [line.strip() for line in code.split('\n')]
        return [line for line in lines if line and not line.startswith('//')]

    def parse_array_literal(self, expr):
        """Parse [1, 2, 3] into a list"""
        expr = expr.strip()
        if expr.startswith('[') and expr.endswith(']'):
            content = expr[1:-1].strip()
            if not content:
                return []
            elements = [self.evaluate_expression(e.strip()) for e in content.split(',')]
            return elements
        return None

    def parse_value(self, value):
        value = value.strip()

        # Array literal
        if value.startswith('['):
            return self.parse_array_literal(value)

        # Array access
        if '[' in value and ']' in value:
            match = re.match(r'(\w+)\[(.+)\]', value)
            if match:
                arr_name = match.group(1)
                index_expr = match.group(2)
                if arr_name in self.variables:
                    index = int(self.evaluate_expression(index_expr))
                    arr = self.variables[arr_name]
                    if isinstance(arr, list) and 0 <= index < len(arr):
                        return arr[index]
                return 0

        # Variable
        if value in self.variables:
            return self.variables[value]

        # Bool
        if value == 'true':
            return 1
        if value == 'false':
            return 0

        # Number
        try:
            if '.' in value:
                return float(value)
            return int(value)
        except ValueError:
            return 0

    def evaluate_expression(self, expr):
        expr = expr.strip()

        # Replace variables with values
        for var, val in self.variables.items():
            if not isinstance(val, list):
                expr = re.sub(r'\b' + re.escape(var) + r'\b', str(val), expr)

        # Handle array literals
        if expr.startswith('['):
            return self.parse_array_literal(expr)

        try:
            return eval(expr)
        except:
            return 0

    def evaluate_condition(self, cond):
        cond = cond.strip()

        # Handle && and ||
        if '&&' in cond:
            parts = cond.split('&&')
            return all(self.evaluate_condition(p) for p in parts)
        if '||' in cond:
            parts = cond.split('||')
            return any(self.evaluate_condition(p) for p in parts)

        # Handle comparison operators
        for op in ['==', '!=', '>=', '<=', '>', '<']:
            if op in cond:
                left, right = cond.split(op, 1)
                left_val = self.evaluate_expression(left)
                right_val = self.evaluate_expression(right)
                if op == '>':
                    return left_val > right_val
                elif op == '<':
                    return left_val < right_val
                elif op == '>=':
                    return left_val >= right_val
                elif op == '<=':
                    return left_val <= right_val
                elif op == '==':
                    return left_val == right_val
                elif op == '!=':
                    return left_val != right_val

        # Bool variable or literal
        val = self.evaluate_expression(cond)
        return bool(val)

    def call_function(self, func_name, args):
        """Handle function calls"""
        if func_name == 'to_float':
            return float(args[0]) if args else 0.0
        elif func_name == 'to_int':
            return int(args[0]) if args else 0
        elif func_name == 'to_bool':
            return 1 if args[0] else 0
        elif func_name == 'length':
            if args and isinstance(args[0], list):
                return len(args[0])
            return 0
        elif func_name == 'max':
            if args and isinstance(args[0], list):
                return max(args[0]) if args[0] else 0
            return 0
        elif func_name == 'abs':
            return abs(args[0]) if args else 0
        elif func_name == 'index':
            if len(args) >= 2 and isinstance(args[0], list):
                try:
                    return args[0].index(args[1])
                except:
                    return -1
            return -1
        elif func_name == 'find':
            # Simplified find - just return first element
            if args and isinstance(args[0], list) and args[0]:
                return args[0][0]
            return -1
        return 0

    def execute_line(self, line):
        if not line or line.startswith('/*') or line.startswith('//'):
            return None

        line = line.rstrip(';').strip()

        # Variable declaration with array
        if line.startswith('array '):
            match = re.match(r'array\s+(\w+)\s*=\s*(.+)', line)
            if match:
                var_name = match.group(1)
                value_expr = match.group(2)
                self.variables[var_name] = self.evaluate_expression(value_expr)
                return None

        # Variable declaration
        if line.startswith('var ') or line.startswith('int ') or line.startswith('float ') or line.startswith('bool '):
            match = re.match(r'(?:var\s+)?(\w+)\s+(?:int|float|bool|array)\s*(?:=\s*(.+))?', line)
            if match:
                var_name = match.group(1)
                value = match.group(2)
                if value:
                    self.variables[var_name] = self.evaluate_expression(value)
                else:
                    self.variables[var_name] = 0
                return None

        # Array element assignment
        if '[' in line and ']' in line and '=' in line:
            match = re.match(r'(\w+)\[(.+)\]\s*=\s*(.+)', line)
            if match:
                arr_name = match.group(1)
                index_expr = match.group(2)
                value_expr = match.group(3)
                if arr_name in self.variables:
                    index = int(self.evaluate_expression(index_expr))
                    value = self.evaluate_expression(value_expr)
                    arr = self.variables[arr_name]
                    if isinstance(arr, list) and 0 <= index < len(arr):
                        arr[index] = value
                return None

        # Assignment
        if '=' in line and not any(op in line for op in ['==', '!=', '>=', '<=']):
            # Compound assignment
            for op in ['+=', '-=', '*=', '/=', '%=']:
                if op in line:
                    var_name, value = line.split(op, 1)
                    var_name = var_name.strip()
                    value = self.evaluate_expression(value)
                    if var_name in self.variables:
                        if op == '+=':
                            self.variables[var_name] += value
                        elif op == '-=':
                            self.variables[var_name] -= value
                        elif op == '*=':
                            self.variables[var_name] *= value
                        elif op == '/=':
                            self.variables[var_name] /= value
                        elif op == '%=':
                            self.variables[var_name] %= value
                    return None

            # Regular assignment
            var_name, value = line.split('=', 1)
            var_name = var_name.strip()
            self.variables[var_name] = self.evaluate_expression(value)
            return None

        # Increment/Decrement
        if line.endswith('++'):
            var = line[:-2].strip()
            if var in self.variables:
                self.variables[var] += 1
            return None

        if line.endswith('--'):
            var = line[:-2].strip()
            if var in self.variables:
                self.variables[var] -= 1
            return None

        # Special operations
        parts = line.split()
        if len(parts) > 0:
            cmd = parts[0]

            # INC/DEC
            if cmd == 'INC' and len(parts) == 2:
                var = parts[1]
                if var in self.variables:
                    self.variables[var] += 1
                return None

            if cmd == 'DEC' and len(parts) == 2:
                var = parts[1]
                if var in self.variables:
                    self.variables[var] -= 1
                return None

            # Binary operations
            if cmd == 'ADD' and len(parts) == 4:
                dest, arg1, arg2 = parts[1], parts[2], parts[3]
                self.variables[dest] = self.parse_value(arg1) + self.parse_value(arg2)
                return None

            if cmd == 'SUB' and len(parts) == 4:
                dest, arg1, arg2 = parts[1], parts[2], parts[3]
                self.variables[dest] = self.parse_value(arg1) - self.parse_value(arg2)
                return None

            if cmd == 'MUL' and len(parts) == 4:
                dest, arg1, arg2 = parts[1], parts[2], parts[3]
                self.variables[dest] = self.parse_value(arg1) * self.parse_value(arg2)
                return None

            if cmd == 'DIV' and len(parts) == 4:
                dest, arg1, arg2 = parts[1], parts[2], parts[3]
                val2 = self.parse_value(arg2)
                if val2 != 0:
                    self.variables[dest] = self.parse_value(arg1) // val2
                return None

            if cmd == 'MOD' and len(parts) == 4:
                dest, arg1, arg2 = parts[1], parts[2], parts[3]
                self.variables[dest] = self.parse_value(arg1) % self.parse_value(arg2)
                return None

            if cmd == 'PLE' and len(parts) == 3:
                dest, arg = parts[1], parts[2]
                if dest in self.variables:
                    self.variables[dest] += self.parse_value(arg)
                return None

            if cmd == 'MIE' and len(parts) == 3:
                dest, arg = parts[1], parts[2]
                if dest in self.variables:
                    self.variables[dest] -= self.parse_value(arg)
                return None

            if cmd == 'AND' and len(parts) == 4:
                dest, arg1, arg2 = parts[1], parts[2], parts[3]
                self.variables[dest] = 1 if (self.parse_value(arg1) and self.parse_value(arg2)) else 0
                return None

            if cmd == 'OR' and len(parts) == 4:
                dest, arg1, arg2 = parts[1], parts[2], parts[3]
                self.variables[dest] = 1 if (self.parse_value(arg1) or self.parse_value(arg2)) else 0
                return None

            # Print
            if cmd.startswith('print('):
                match = re.match(r'print\((.+)\)', line)
                if match:
                    expr = match.group(1)
                    value = self.evaluate_expression(expr)
                    print(int(value))
                return None

            # Function calls
            if cmd in ['to_float', 'to_int', 'to_bool', 'length', 'max', 'abs', 'index', 'find']:
                match = re.match(r'(\w+)\((.+)\)', line)
                if match:
                    func_name = match.group(1)
                    args_str = match.group(2)
                    # Parse arguments
                    args = []
                    for arg in args_str.split(','):
                        arg = arg.strip()
                        if arg in self.variables:
                            args.append(self.variables[arg])
                        else:
                            args.append(self.parse_value(arg))
                    return self.call_function(func_name, args)

        return None

    def execute(self, lines):
        i = 0
        while i < len(lines):
            line = lines[i].strip()

            # If statement
            if line.startswith('if '):
                i = self.execute_if_block(lines, i)
                continue

            # For loop
            if line.startswith('for '):
                i = self.execute_for_block(lines, i)
                continue

            # Foreach loop
            if line.startswith('foreach'):
                i = self.execute_foreach_block(lines, i)
                continue

            # Match statement
            if line.startswith('match '):
                i = self.execute_match_block(lines, i)
                continue

            # Regular statement
            self.execute_line(line)
            i += 1

    def execute_if_block(self, lines, start_idx):
        """Execute an if-else block"""
        if_line = lines[start_idx]
        match = re.search(r'if\s*\((.+)\)\s*\{', if_line)
        if not match:
            return start_idx + 1

        condition = match.group(1)

        # Find the matching }
        i = start_idx + 1
        depth = 1
        if_body = []

        while i < len(lines) and depth > 0:
            line = lines[i]
            if depth == 1 and '}' in line and 'else' in line:
                break
            if '{' in line:
                depth += 1
            if '}' in line:
                depth -= 1
                if depth == 0:
                    break
            if_body.append(line)
            i += 1

        next_idx = i if 'else' in lines[i] else i + 1

        # Execute if condition is true
        if self.evaluate_condition(condition):
            self.execute(if_body)
            # Skip all else/else if blocks
            while next_idx < len(lines):
                next_line = lines[next_idx].strip()
                if 'else if' in next_line or ('else' in next_line and '{' in next_line):
                    if '{' in next_line:
                        next_idx += 1
                        depth = 1
                        while next_idx < len(lines) and depth > 0:
                            if '{' in lines[next_idx]:
                                depth += 1
                            if '}' in lines[next_idx]:
                                depth -= 1
                            next_idx += 1
                else:
                    break
            return next_idx

        # Condition was false, check for else if / else
        if next_idx < len(lines):
            next_line = lines[next_idx].strip()

            if 'else if' in next_line:
                return self.execute_if_block(lines, next_idx)

            elif 'else' in next_line and '{' in next_line:
                i = next_idx + 1
                depth = 1
                else_body = []
                while i < len(lines) and depth > 0:
                    line = lines[i]
                    if '{' in line:
                        depth += 1
                    if '}' in line:
                        depth -= 1
                        if depth == 0:
                            break
                    else_body.append(line)
                    i += 1
                self.execute(else_body)
                return i + 1

        return next_idx

    def execute_for_block(self, lines, start_idx):
        """Execute a for loop"""
        for_line = lines[start_idx]
        match = re.match(r'for\s*\(\s*(?:int\s+)?(\w+)\s*=\s*(.+?)\s*;\s*(.+?)\s*;\s*(.+?)\)\s*\{', for_line)
        if not match:
            return start_idx + 1

        var_name = match.group(1)
        init_val = match.group(2)
        condition = match.group(3)
        increment = match.group(4)

        # Initialize
        self.variables[var_name] = self.evaluate_expression(init_val)

        # Find loop body
        i = start_idx + 1
        depth = 1
        body = []

        while i < len(lines) and depth > 0:
            line = lines[i]
            if '{' in line:
                depth += 1
            if '}' in line:
                depth -= 1
                if depth == 0:
                    break
            body.append(line)
            i += 1

        # Execute loop
        while self.evaluate_condition(condition):
            self.execute(body)
            self.execute_line(increment)

        return i + 1

    def execute_foreach_block(self, lines, start_idx):
        """Execute a foreach loop"""
        foreach_line = lines[start_idx]
        match = re.match(r'foreach\s*\(\s*(\w+)\s+in\s+(\w+)\s*\)\s*\{', foreach_line)
        if not match:
            return start_idx + 1

        var_name = match.group(1)
        array_name = match.group(2)

        if array_name not in self.variables:
            return start_idx + 1

        # Find loop body
        i = start_idx + 1
        depth = 1
        body = []

        while i < len(lines) and depth > 0:
            line = lines[i]
            if '{' in line:
                depth += 1
            if '}' in line:
                depth -= 1
                if depth == 0:
                    break
            body.append(line)
            i += 1

        # Execute loop
        arr = self.variables[array_name]
        if isinstance(arr, list):
            for elem in arr:
                self.variables[var_name] = elem
                self.execute(body)

        return i + 1

    def execute_match_block(self, lines, start_idx):
        """Execute a match statement"""
        match_line = lines[start_idx]
        match_obj = re.match(r'match\s+(\w+)\s*\{', match_line)
        if not match_obj:
            return start_idx + 1

        var_name = match_obj.group(1)
        value = self.variables.get(var_name, 0)

        # Find match body
        i = start_idx + 1
        depth = 1
        cases = []

        while i < len(lines) and depth > 0:
            line = lines[i]
            if '{' in line:
                depth += 1
            if '}' in line:
                depth -= 1
                if depth == 0:
                    break

            # Parse case: value -> action
            case_match = re.match(r'(.+?)\s*->\s*(.+)', line.rstrip(','))
            if case_match:
                case_val = case_match.group(1).strip()
                action = case_match.group(2).strip()
                cases.append((case_val, action))

            i += 1

        # Execute matching case
        for case_val, action in cases:
            if case_val == '_' or int(self.evaluate_expression(case_val)) == value:
                self.execute_line(action)
                break

        return i + 1

    def run(self, code):
        lines = self.tokenize(code)
        self.execute(lines)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python interpreter_v2.py <file>")
        sys.exit(1)

    with open(sys.argv[1], 'r') as f:
        code = f.read()

    interpreter = Interpreter()
    interpreter.run(code)
