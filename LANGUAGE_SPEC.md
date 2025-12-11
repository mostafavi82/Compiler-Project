# Language Specification - Phase 1

## مشخصات زبان - فاز اول

### 1. انواع داده (Data Types)

#### 1.1 انواع پایه
- `int` - اعداد صحیح (پشتیبانی کامل)
- `bool` - مقادیر منطقی: `true` یا `false`
- `float` - اعداد اعشاری (مثال: `3.14`)
- `array` - آرایه‌ها

### 2. تعریف متغیر (Variable Declaration)

#### 2.1 سینتکس
```
var <name> <type> [= <value>];
<type> <name> [= <value>];
```

#### 2.2 مثال‌ها
```c
var x int;              // تعریف بدون مقداردهی
var y int = 5;          // تعریف با مقداردهی
int z = 10;             // بدون var
var flag bool = true;   // نوع bool
var pi float = 3.14;    // نوع float
array arr = [1, 2, 3];  // آرایه
```

#### 2.3 قوانین نام‌گذاری
- باید با حرف یا `_` شروع شود
- می‌تواند شامل حروف، اعداد و `_` باشد
- نمی‌تواند کلمه کلیدی باشد

### 3. دستورات انتساب خاص (Special Assignment Statements)

#### 3.1 دستورات سه‌تایی (ternary)
```
ADD x y z    // x = y + z
SUB x y z    // x = y - z
MUL x y z    // x = y * z
DIV x y z    // x = y / z
MOD x y z    // x = y % z
```

**نکته**: همه آرگومان‌ها باید متغیر باشند (نه عدد مستقیم).

#### 3.2 دستورات یکانی (unary)
```
INC x    // x = x + 1
DEC x    // x = x - 1
```

#### 3.3 دستورات دوتایی (binary)
```
PLE x y    // x = x + y
MIE x y    // x = x - y
```

#### 3.4 دستورات منطقی (فقط برای bool)
```
AND x y z    // x = y && z
OR x y z     // x = y || z
```

### 4. دستورات شرطی (Conditional Statements)

#### 4.1 سینتکس if-else
```c
if (<condition>) {
    <statements>
} [else if (<condition>) {
    <statements>
}]* [else {
    <statements>
}]
```

#### 4.2 عملگرهای مقایسه
- `==` - مساوی
- `!=` - نامساوی
- `>` - بزرگتر
- `<` - کوچکتر
- `>=` - بزرگتر مساوی
- `<=` - کوچکتر مساوی

#### 4.3 عملگرهای منطقی
- `&&` - AND منطقی
- `||` - OR منطقی

#### 4.4 مثال
```c
if (x > 5) {
    print(x);
} else if (x > 0) {
    print(0);
} else {
    print(-1);
}

if (x > 5 && y < 10) {
    print(100);
}
```

### 5. حلقه‌ها (Loops)

#### 5.1 حلقه for
```c
for (<init>; <condition>; <increment>) {
    <statements>
}
```

**مثال:**
```c
for (int i = 0; i < 10; i++) {
    print(i);
}
```

#### 5.2 حلقه foreach
```c
foreach (<var> in <array>) {
    <statements>
}
```

**مثال:**
```c
array numbers = [1, 2, 3, 4, 5];
foreach (num in numbers) {
    print(num);
}
```

### 6. Pattern Matching

#### 6.1 سینتکس
```c
match <expression> {
    <pattern1> -> <statement>,
    <pattern2> -> <statement>,
    _ -> <statement>
}
```

#### 6.2 مثال
```c
var x int = 2;

match x {
    0 -> print(0),
    1 -> print(1),
    2 -> print(2),
    _ -> print(999)
}
```

**نکته**: `_` برای حالت پیش‌فرض (default) استفاده می‌شود.

### 7. توابع Built-in

#### 7.1 تابع print
```c
print(<expression>);
```

چاپ مقدار یک عبارت.

#### 7.2 توابع تبدیل نوع
```c
to_int(<expression>)
to_float(<expression>)
to_bool(<expression>)
```

#### 7.3 توابع ریاضی
```c
abs(<expression>)    // قدر مطلق
```

#### 7.4 توابع آرایه
```c
length(<array>)           // طول آرایه
max(<array>)              // بزرگترین عنصر
index(<array>, <index>)   // دسترسی به عنصر
find(<array>, <condition>) // جستجو
```

### 8. آرایه‌ها (Arrays)

#### 8.1 تعریف
```c
array arr = [1, 2, 3, 4, 5];
```

#### 8.2 دسترسی به عناصر
```c
var x int = arr[0];    // عنصر اول
var y int = arr[4];    // عنصر آخر
```

#### 8.3 Array Comprehension
```c
array evens = [x * 2 for x in nums if x % 2 == 0];
```

### 9. کامنت‌ها (Comments)

```c
/* تک‌خطی */

/* چند
   خطی */
```

**نکته**: فقط کامنت بلوکی `/* */` پشتیبانی می‌شود.

### 10. عملگرهای ریاضی

در عبارت‌ها می‌توان از عملگرهای زیر استفاده کرد:
- `+` - جمع
- `-` - تفریق
- `*` - ضرب
- `/` - تقسیم
- `%` - باقی‌مانده
- `^` - توان (در نسخه قدیمی)

**مثال:**
```c
var result int = 2 + 3 * 4;
var x int = (a + b) * c;
```

### 11. اولویت عملگرها

از بالا به پایین (بیشترین تا کمترین):
1. `()` - پرانتز
2. `^` - توان
3. `*`, `/`, `%` - ضرب، تقسیم، باقی‌مانده
4. `+`, `-` - جمع، تفریق
5. `>`, `<`, `>=`, `<=`, `==`, `!=` - مقایسه
6. `&&` - AND منطقی
7. `||` - OR منطقی

### 12. کلمات کلیدی (Reserved Keywords)

```
var, int, bool, float, array
true, false
if, else, for, foreach, in, match
print
ADD, SUB, MUL, DIV, MOD
INC, DEC, PLE, MIE
AND, OR
to_int, to_float, to_bool
abs, length, max, index, find
```

### 13. محدودیت‌ها

1. **متغیرهای محلی در بلوک‌ها**: نمی‌توان داخل بدنه if یا حلقه متغیر جدید تعریف کرد
2. **دستورات خاص در شرط‌ها**: از ADD, SUB و غیره نمی‌توان در شرط‌های if یا for استفاده کرد
3. **تو در تو**: if و حلقه‌ها می‌توانند تو در تو باشند

### 14. نمونه برنامه کامل

```c
/* Calculate factorial */

var n int = 5;
var result int = 1;

for (int i = 1; i <= n; i++) {
    MUL result result i;
}

print(result);
```
