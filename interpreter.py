#!/usr/bin/env python3
"""
Simple interpreter for the custom language
This is a quick implementation to test the syntax without needing LLVM
"""

import re
import sys

class Interpreter:
    def __init__(self):
        self.variables = {}

    def remove_comments(self, code):
        """Remove /* ... */ comments"""
        return re.sub(r'/\*.*?\*/', '', code, flags=re.DOTALL)

    def tokenize(self, code):
        """Split code into lines and clean up"""
        code = self.remove_comments(code)
        lines = [line.strip() for line in code.split('\n')]
        return [line for line in lines if line and not line.startswith('//')]

    def parse_value(self, value):
        """Parse a value (can be variable name or literal)"""
        value = value.strip()

        # Check if it's a variable
        if value in self.variables:
            return self.variables[value]

        # Check if it's true/false
        if value == 'true':
            return 1
        if value == 'false':
            return 0

        # Try to parse as number
        try:
            if '.' in value:
                return float(value)
            return int(value)
        except ValueError:
            # It's an undefined variable
            return 0

    def evaluate_expression(self, expr):
        """Evaluate a simple arithmetic expression"""
        expr = expr.strip()

        # Handle parentheses first (simplified)
        while '(' in expr:
            # Find innermost parentheses
            match = re.search(r'\(([^()]+)\)', expr)
            if match:
                inner = match.group(1)
                result = self.evaluate_expression(inner)
                expr = expr[:match.start()] + str(result) + expr[match.end():]
            else:
                break

        # Replace variables with their values
        for var, val in self.variables.items():
            expr = re.sub(r'\b' + re.escape(var) + r'\b', str(val), expr)

        # Evaluate the expression
        try:
            # Simple eval for arithmetic
            return eval(expr)
        except:
            return 0

    def evaluate_condition(self, cond):
        """Evaluate a boolean condition"""
        cond = cond.strip()

        # Handle logical operators
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

                if op == '==':
                    return left_val == right_val
                elif op == '!=':
                    return left_val != right_val
                elif op == '>':
                    return left_val > right_val
                elif op == '<':
                    return left_val < right_val
                elif op == '>=':
                    return left_val >= right_val
                elif op == '<=':
                    return left_val <= right_val

        # If no operator found, evaluate as expression
        return bool(self.evaluate_expression(cond))

    def execute_line(self, line):
        """Execute a single line"""
        line = line.strip()

        if not line or line.endswith('{') or line == '}':
            return None

        # Remove semicolon
        if line.endswith(';'):
            line = line[:-1].strip()

        # Variable declaration: var x int = 5;
        if line.startswith('var ') or line.startswith('int ') or line.startswith('float ') or line.startswith('bool '):
            match = re.match(r'(?:var\s+)?(\w+)\s+(?:int|float|bool)\s*(?:=\s*(.+))?', line)
            if match:
                var_name = match.group(1)
                value = match.group(2)
                if value:
                    self.variables[var_name] = self.evaluate_expression(value)
                else:
                    self.variables[var_name] = 0
                return None

        # Assignment: x = 5;
        if '=' in line and not any(op in line for op in ['==', '!=', '>=', '<=']):
            # Check for compound assignment
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
            if '=' in line:
                var_name, value = line.split('=', 1)
                var_name = var_name.strip()
                self.variables[var_name] = self.evaluate_expression(value)
                return None

        # Check for x++ or x--
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

            # INC x
            if cmd == 'INC' and len(parts) == 2:
                var = parts[1]
                if var in self.variables:
                    self.variables[var] += 1
                return None

            # DEC x
            if cmd == 'DEC' and len(parts) == 2:
                var = parts[1]
                if var in self.variables:
                    self.variables[var] -= 1
                return None

            # ADD x y z -> x = y + z
            if cmd == 'ADD' and len(parts) == 4:
                dest, arg1, arg2 = parts[1], parts[2], parts[3]
                self.variables[dest] = self.parse_value(arg1) + self.parse_value(arg2)
                return None

            # SUB x y z -> x = y - z
            if cmd == 'SUB' and len(parts) == 4:
                dest, arg1, arg2 = parts[1], parts[2], parts[3]
                self.variables[dest] = self.parse_value(arg1) - self.parse_value(arg2)
                return None

            # MUL x y z -> x = y * z
            if cmd == 'MUL' and len(parts) == 4:
                dest, arg1, arg2 = parts[1], parts[2], parts[3]
                self.variables[dest] = self.parse_value(arg1) * self.parse_value(arg2)
                return None

            # DIV x y z -> x = y / z
            if cmd == 'DIV' and len(parts) == 4:
                dest, arg1, arg2 = parts[1], parts[2], parts[3]
                val2 = self.parse_value(arg2)
                if val2 != 0:
                    self.variables[dest] = self.parse_value(arg1) // val2
                return None

            # MOD x y z -> x = y % z
            if cmd == 'MOD' and len(parts) == 4:
                dest, arg1, arg2 = parts[1], parts[2], parts[3]
                self.variables[dest] = self.parse_value(arg1) % self.parse_value(arg2)
                return None

            # PLE x y -> x += y
            if cmd == 'PLE' and len(parts) == 3:
                dest, arg = parts[1], parts[2]
                if dest in self.variables:
                    self.variables[dest] += self.parse_value(arg)
                return None

            # MIE x y -> x -= y
            if cmd == 'MIE' and len(parts) == 3:
                dest, arg = parts[1], parts[2]
                if dest in self.variables:
                    self.variables[dest] -= self.parse_value(arg)
                return None

            # print(x)
            if cmd.startswith('print('):
                match = re.match(r'print\((.+)\)', line)
                if match:
                    expr = match.group(1)
                    value = self.evaluate_expression(expr)
                    print(int(value))
                return None

        return None

    def execute_if_block(self, lines, start_idx):
        """Execute an if-else block and return the index after the block"""
        # Parse: if (condition) { or } else if (condition) {
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
            # Check if this line has } followed by else - that's the end of this if block
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
                    # Skip this block
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

            # Handle "} else if" or "else if"
            if 'else if' in next_line:
                # Recursively handle else if
                return self.execute_if_block(lines, next_idx)

            # Handle "} else {" or "else {"
            elif 'else' in next_line and '{' in next_line:
                # Execute else block
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
        """Execute a for loop and return the index after the block"""
        # Parse: for (int i = 0; i < 10; i++) {
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
            # Execute increment
            self.execute_line(increment)

        return i + 1

    def execute(self, lines):
        """Execute a list of lines"""
        if isinstance(lines, str):
            lines = self.tokenize(lines)

        i = 0
        while i < len(lines):
            line = lines[i].strip()

            # Handle if statements
            if line.startswith('if '):
                i = self.execute_if_block(lines, i)
                continue

            # Handle for loops
            if line.startswith('for '):
                i = self.execute_for_block(lines, i)
                continue

            # Regular statement
            self.execute_line(line)
            i += 1

    def run(self, code):
        """Run the code"""
        lines = self.tokenize(code)
        self.execute(lines)

if __name__ == '__main__':
    if len(sys.argv) > 1:
        # Read from file
        with open(sys.argv[1], 'r') as f:
            code = f.read()
    else:
        # Read from stdin
        code = sys.stdin.read()

    interpreter = Interpreter()
    interpreter.run(code)
