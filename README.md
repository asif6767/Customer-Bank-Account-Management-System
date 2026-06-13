Banking-Management-System/
│
├── 📄 README.md
├── 📄 LICENSE
├── 📄 .gitignore
│
├── 📁 src/
│   └── main.c
│
├── 📁 include/
│   └── banking.h
│
├── 📁 data/
│   └── record.dat
│
├── 📁 docs/
│   ├── flowchart.png
│   ├── system-design.md
│   └── screenshots/
│       └── menu.png
│
├── 📁 build/
│   └── (compiled files - ignored in git)
│
└── 📁 assets/
    └── banner.png

# 🏦 Customer Account Banking Management System (C Project)

A simple console-based **Banking Management System** written in C language.
This project demonstrates the use of **structures, file handling, functions, loops, and conditionals** to simulate real-world banking operations.

---

# 📌 Features

* Create new bank account
* View all customers
* Search account details
* Edit account information
* Deposit & withdraw money
* Delete account
* Interest calculation (Fixed/Saving accounts)
* Password protected login system

---

# 🔐 Login System

The program starts with a password check:

```c
char password[10] = "jubayer";
```

User must enter correct password to access the system.

---

# ⚙️ Overall Flow of the Program

```
main()
   │
   ▼
Password Verification
   │
   ▼
menu()
   │
 ┌─────────────── Banking Operations ───────────────┐
 │ 1. new_acc()                                     │
 │ 2. edit()                                        │
 │ 3. transact()                                    │
 │ 4. see()                                         │
 │ 5. erase()                                       │
 │ 6. view_list()                                   │
 │ 7. close()                                       │
 └──────────────────────────────────────────────────┘
   │
   ▼
record.dat (File Storage)
```

---

# 📁 File Handling Used

This project uses file handling to store permanent customer data.

### Functions Used:

| Function    | Purpose                         |
| ----------- | ------------------------------- |
| `fopen()`   | Opens file (read/write/append)  |
| `fclose()`  | Closes file                     |
| `fprintf()` | Writes formatted data into file |
| `fscanf()`  | Reads formatted data from file  |
| `remove()`  | Deletes old file                |
| `rename()`  | Renames temporary file          |

### Main File:

```
record.dat → Stores all customer records
```

---

# 🧮 Formula Used

### 💰 Simple Interest Formula

Used for Fixed and Saving accounts:

```
SI = (P × R × T) / 100
```

Where:

* P = Principal amount
* R = Rate of interest
* T = Time (years)

---

# 🧩 All Functions Explained

## 1. `main()`

* Starts program
* Checks password
* Calls `menu()`

---

## 2. `menu()`

* Displays main menu
* Redirects user to different functions using switch-case

---

## 3. `new_acc()`

* Creates new bank account
* Stores user data in `record.dat`

---

## 4. `view_list()`

* Shows list of all customers
* Displays account number, name, address, phone

---

## 5. `edit()`

* Updates customer information
* Allows change of:

  * Address
  * Phone number

---

## 6. `transact()`

* Handles banking transactions
* Supports:

  * Deposit
  * Withdraw
* Updates balance in file

---

## 7. `erase()`

* Deletes an existing account
* Removes record from `record.dat`

---

## 8. `see()`

* Searches account by:

  * Account number
  * Name
* Displays full account details
* Calculates interest if applicable

---

## 9. `interest()`

* Calculates simple interest
* Used for fixed and saving accounts

---

## 10. `fordelay()`

* Creates delay effect (loading animation)
* Used for UI simulation

---

## 11. `close()`

* Exits program
* Displays developer message

---

## 12. `system() calls`

Used for:

* `system("cls")` → Clear screen
* `system("color 9")` → Change console color

---

# 🏦 Account Types Supported

* Saving Account → 8% interest
* Current Account → No interest
* Fixed1 → 1 year (9%)
* Fixed2 → 2 years (11%)
* Fixed3 → 3 years (13%)

---

# 📊 Example Interest Calculation

If:

```
P = 10000
R = 9%
T = 1 year
```

Then:

```
SI = (10000 × 9 × 1) / 100 = 900
```

---

# 🧠 Key Concepts Used

* Structures (`struct`)
* File Handling
* Functions
* Switch-case
* Loops
* String comparison (`strcmp`)
* Conditional logic
* Windows console control

---

# 🚀 Conclusion

This project is a basic simulation of a banking system using C programming.
It demonstrates how real-world applications can be built using:

* Modular functions
* File-based data storage
* Basic financial calculations

---

👨‍💻 Developed for learning purposes (C Mini Project)



