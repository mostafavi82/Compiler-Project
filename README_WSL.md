# راهنمای اجرا با WSL (Windows Subsystem for Linux)

## نصب و راه‌اندازی

### 1. باز کردن WSL

از CMD یا PowerShell:
```cmd
wsl
```

یا از Git Bash:
```bash
wsl.exe
```

### 2. رفتن به پوشه پروژه

```bash
cd /mnt/c/Users/Hossein/Documents/Compiler-Project
```

### 3. Build کردن پروژه

```bash
./build_wsl.sh
```

این اسکریپت:
- بررسی می‌کنه که cmake، clang، و llvm نصب باشن
- اگر نباشن، نصبشون می‌کنه
- پروژه رو build می‌کنه

### 4. اجرای کامپایلر

```bash
./run_wsl.sh
```

این اسکریپت:
- فایل input.txt رو کامپایل می‌کنه
- LLVM IR تولید می‌کنه
- اجرایی می‌سازه و run می‌کنه
- خروجی رو نشون می‌ده

## اجرای دستی

اگر می‌خوای step به step خودت اجرا کنی:

```bash
# Build
cd build/src

# Compile
./compiler "$(cat ../../input.txt)" > compiler.ll

# Generate object file
llc --filetype=obj -o=compiler.o compiler.ll

# Link
clang -o compilerbin compiler.o ../../rtCompiler.c

# Run
./compilerbin
```

## تست مثال‌ها

```bash
# Example 1
./compiler "$(cat example1.txt)" > ex1.ll
llc --filetype=obj -o=ex1.o ex1.ll
clang -o ex1 ex1.o rtCompiler.c
./ex1

# Example 2
./compiler "$(cat example2.txt)" > ex2.ll
llc --filetype=obj -o=ex2.o ex2.ll
clang -o ex2 ex2.o rtCompiler.c
./ex2

# Example 3
./compiler "$(cat example3.txt)" > ex3.ll
llc --filetype=obj -o=ex3.o ex3.ll
clang -o ex3 ex3.o rtCompiler.c
./ex3
```

## مقایسه با مفسر Python

می‌تونی خروجی کامپایلر LLVM رو با مفسر Python مقایسه کنی:

```bash
# LLVM output
./run_wsl.sh

# Python output
python3 interpreter.py input.txt
```

هر دو باید خروجی یکسانی بدن!

## عیب‌یابی

### خطا: command not found

اگر `./build_wsl.sh` یا `./run_wsl.sh` اجرا نشد:

```bash
chmod +x build_wsl.sh run_wsl.sh
```

### خطا: Syntax errors occurred

ببین کد input.txt چیه:
```bash
cat input.txt
```

تست با کد ساده:
```bash
echo "var x int = 5;" > test.txt
./compiler "$(cat test.txt)"
```

### خطا: undefined reference to main

یعنی کامپایلر LLVM IR تولید نکرده. چک کن:
```bash
cat build/src/compiler.ll
```

اگر خالی بود، مشکل از Parser هست.
