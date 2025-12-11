#!/usr/bin/env python3
"""
Comprehensive test suite for the compiler
Tests all examples and verifies output
"""

import subprocess
import sys

def run_test(filename, expected_output):
    """Run a test file and check output"""
    try:
        result = subprocess.run(
            ['python', 'interpreter.py', filename],
            capture_output=True,
            text=True,
            timeout=5
        )

        output = result.stdout.strip()
        expected = expected_output.strip()

        if output == expected:
            print(f"✓ {filename}: PASSED")
            return True
        else:
            print(f"✗ {filename}: FAILED")
            print(f"  Expected:\n{expected}")
            print(f"  Got:\n{output}")
            return False
    except subprocess.TimeoutExpired:
        print(f"✗ {filename}: TIMEOUT")
        return False
    except Exception as e:
        print(f"✗ {filename}: ERROR - {e}")
        return False

def main():
    print("=" * 60)
    print("Running Compiler Test Suite")
    print("=" * 60)
    print()

    tests = [
        ("input.txt", "6\n16\n100"),
        ("example1.txt", "30\n10\n30\n11\n19\n16"),
        ("example2.txt", "100\n2\n999"),
        ("example3.txt", "55\n10\n9\n8\n7\n6\n5\n4\n3\n2\n1"),
    ]

    passed = 0
    failed = 0

    for filename, expected in tests:
        if run_test(filename, expected):
            passed += 1
        else:
            failed += 1
        print()

    print("=" * 60)
    print(f"Results: {passed} passed, {failed} failed")
    print("=" * 60)

    return 0 if failed == 0 else 1

if __name__ == '__main__':
    sys.exit(main())
