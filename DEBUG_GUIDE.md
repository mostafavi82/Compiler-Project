# راهنمای Debug کردن مشکل Build

## مشکل فعلی
خطای `Syntax errors occurred` یعنی Parser نتونسته کد رو پارس کنه و به همین دلیل LLVM IR تولید نمی‌شه.

## مراحل Debug:

### 1. ببینید دقیقاً کجا خطا می‌ده

دستور زیر رو اجرا کنید تا خروجی کامل رو ببینید:

```bash
cd build/src
./compiler "$(cat ../../input.txt)" 2>&1 | tee output.log
cat output.log
```

اگر چیزی شبیه `Unexpected: ...` دیدید، اون token مشکل‌ساز هست.

### 2. تست با کد ساده

یه فایل test ساده بسازید:

```bash
echo "var x int = 5;" > test1.txt
./compiler "$(cat test1.txt)"
```

اگر این کار کرد، کم کم خطوط دیگه رو اضافه کنید:

```bash
echo "var x int = 5;
INC x;" > test2.txt
./compiler "$(cat test2.txt)"
```

### 3. چک کردن compiler.ll

بعد از run کردن compiler، چک کنید compiler.ll چی توش هست:

```bash
cat compiler.ll
```

اگر خالی بود یا فقط یه خط داشت، یعنی Parser خطا داده.

### 4. مشکلات احتمالی و راه‌حل‌ها

#### مشکل 1: Token نشناخته

اگر پیغام `Unexpected: something` دیدید، یعنی:
- یا Lexer token رو نمی‌شناسه
- یا Parser انتظار token دیگه‌ای رو داره

**راه‌حل**: پیغام دقیق رو بفرستید تا بررسی کنیم.

#### مشکل 2: Semicolon مفقود

Parser انتظار semicolon داره بعد از هر statement.

**چک کنید**: همه خطوط با `;` تموم شده باشن.

#### مشکل 3: Comment پارس نمی‌شه

اگر کامنت‌ها مشکل دارن، موقتاً حذفشون کنید:

```bash
sed 's|/\*.*\*/||g' input.txt > input_no_comments.txt
./compiler "$(cat input_no_comments.txt)"
```

### 5. تست با مفسر Python

برای مقایسه، همیشه می‌تونید با Python تست کنید:

```bash
python3 interpreter.py input.txt
```

اگر Python اجرا شد ولی کامپایلر خطا داد، یعنی مشکل از Parser/CodeGen هست.

## خطاهای رایج

### خطا: `Unexpected: {`

مشکل: Parser انتظار چیز دیگه‌ای رو داره قبل از `{`.

**راه‌حل**: چک کنید سینتکس if/for درست باشه.

### خطا: `Unexpected: ;`

مشکل: Parser فکر می‌کنه statement تموم نشده.

**راه‌حل**: چک کنید statement قبلش کامل باشه.

### خطا: `Unexpected: <identifier>`

مشکل: Parser identifier رو نمی‌شناسه.

**راه‌حل**: چک کنید متغیر define شده باشه.

## در صورت نیاز به کمک

لطفاً این اطلاعات رو بفرستید:

1. خروجی دقیق `./compiler`:
```bash
./compiler "$(cat ../../input.txt)" 2>&1
```

2. محتوای input.txt

3. محتوای compiler.ll (اگر تولید شد)

4. نسخه‌ی LLVM:
```bash
llvm-config --version
```
