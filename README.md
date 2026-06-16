# 🏦 Customer Account Banking Management System

A console-based banking app written in **C**. It handles customer accounts, transactions, interest calculations, and login — all stored in CSV files with hashed passwords.

---

## ✨ Features

- Secure login with SHA-256 hashed passwords
- Create, update, and remove customer accounts
- Deposit and withdraw with balance tracking
- Interest calculation based on account type
- Search accounts by number or name
- View all active customers in a list
- Every change is logged — nothing is permanently deleted
- Duplicate account number and User ID protection

---

## 📁 Files & What They Do

| File | What it does |
|------|-------------|
| `banking.h` | Central header — structs, globals, all function prototypes |
| `main.c` | Entry point, starts the login screen |
| `globals.c` | Shared global variables used across all files |
| `security.c` | SHA-256 engine, saves and verifies hashed credentials |
| `utils.c` | Interest formula, delay function, navigation helpers |
| `login.c` | Login screen and new user registration |
| `menu.c` | Main menu, routes to each feature |
| `new_acc.c` | Creates a new customer account |
| `edit.c` | Updates account info (address, phone, age) |
| `transact.c` | Handles deposits and withdrawals |
| `see.c` | View account details and interest projection |
| `erase.c` | Soft-deletes and archives an account |
| `view_list.c` | Shows a table of all active accounts |
| `excel.c` | All CSV read/write operations |
| `close_app.c` | Clean exit with goodbye message |

---

## 🔧 Functions & What They Do

| Function | File | Purpose |
|----------|------|---------|
| `show_login` | `login.c` | Welcome screen, login or register |
| `menu` | `menu.c` | Main menu dispatcher |
| `open_account` | `new_acc.c` | Create new customer account |
| `update_account` | `edit.c` | Edit address, phone, or age |
| `do_transaction` | `transact.c` | Deposit or withdraw money |
| `view_account` | `see.c` | View account details + interest |
| `remove_account` | `erase.c` | Archive and soft-delete account |
| `list_accounts` | `view_list.c` | Print all active accounts |
| `calc_interest` | `utils.c` | Calculates simple interest |
| `cpu_pause` | `utils.c` | Short delay between screens |
| `nav_simple` | `utils.c` | After action: menu or exit |
| `nav_prompt` | `utils.c` | After error: retry, menu, or exit |
| `bx_digest_hex` | `security.c` | Hash a string to SHA-256 hex |
| `bx_store_cred` | `security.c` | Save hashed credentials to file |
| `bx_verify_cred` | `security.c` | Check login credentials |
| `bx_uid_taken` | `security.c` | Check if a User ID already exists |
| `csv_create_account` | `excel.c` | Write new account CSV file |
| `csv_append_txn` | `excel.c` | Log a transaction row |
| `csv_append_update` | `excel.c` | Log a field change row |
| `csv_mark_removed` | `excel.c` | Append removal record |
| `csv_rebuild_master` | `excel.c` | Rebuild `all_customers.csv` |
| `exit_system` | `close_app.c` | Exit the application |

---

## 🔐 Security System

Passwords are **never stored as plain text**. Both the User ID and password go through a custom SHA-256 implementation and only the 64-character hex hash is saved in `credentials.dat`.

```
credentials.dat format:
1001 | uid_hash (64 chars) | pwd_hash (64 chars)
```

- No external crypto library — SHA-256 is written from scratch in `security.c`
- When an account is removed, the confirming User ID is hashed and stored in the archive for audit
- Duplicate User IDs are blocked before any record is created

---

## ⚙️ Algorithms Used

**SHA-256** — Custom implementation for password hashing. Pads input into 512-bit blocks, runs 64 compression rounds using bitwise operations (Ch, Maj, Σ0, Σ1, σ0, σ1), and produces a 256-bit digest.

**Simple Interest** — Used for all account interest calculations.

**Linear Search** — Name-based account lookup scans all CSV files one by one.

**Case-insensitive String Search** — Custom `icase_search()` for partial name matching without POSIX dependency.

**In-place File Rewriting** — File is loaded into memory, the target row is replaced, then written back — used when updating balance or account info.

---

## 💳 Account Types

| Type | Rate | Term | Withdraw |
|------|:----:|------|---------|
| `Saving` | 8% / year | Monthly projection | Anytime |
| `Current` | 0% | No interest | Anytime |
| `Fixed1` | 9% / year | 1 year | After 1 year |
| `Fixed2` | 11% / year | 2 years | After 2 years |
| `Fixed3` | 13% / year | 3 years | After 3 years |

> Fixed accounts block all deposits and withdrawals through the transaction menu.

---

## 📐 Interest Formula & Examples

```
Interest = (Principal × Rate × Time) / 100
Maturity = Principal + Interest
```

| Account | Principal | Rate | Time | Interest | Maturity |
|---------|----------:|:----:|------|:--------:|:--------:|
| Saving | $5,000 | 8% | 1 month | $33.33 | $5,033.33 |
| Fixed1 | $10,000 | 9% | 1 yr | $900.00 | $10,900.00 |
| Fixed2 | $10,000 | 11% | 2 yrs | $2,200.00 | $12,200.00 |
| Fixed3 | $10,000 | 13% | 3 yrs | $3,900.00 | $13,900.00 |

---

## ⌨️ Input Guide

| Field | Type | Example |
|-------|------|---------|
| Account number | Integer | `1001` |
| First / Last name | String (no spaces) | `John` |
| Date (any) | mm/dd/yyyy | `06/15/2025` |
| Age | Integer | `29` |
| Current / Permanent address | Full line (spaces OK) | `123 Main Street` |
| Citizenship number | String | `NID1234567` |
| Phone number | Digits only | `01712345678` |
| Opening deposit / Amount | Decimal | `5000.00` |
| Account type | Exact string | `Saving` `Current` `Fixed1` `Fixed2` `Fixed3` |
| User ID / Password | String (no spaces) | `john_01` / `Pass99` |
| Menu choice | Integer | `1` |

**Things to avoid:**
- Don't type `$` before amounts — numbers only
- Don't use spaces in name fields
- Don't use dashes in phone numbers
- Account type must be one of the five exact values above
- Dates must be `mm/dd/yyyy` — not `5 Jun 2025`

---

## 🔑 Key Concepts

- **Struct-based data model** — `struct account` and `struct date` carry all customer data
- **File-per-account storage** — each customer gets their own `user_<acc_no>.csv`
- **Append-only audit log** — updates and transactions are appended, never overwritten
- **Soft delete** — removed accounts are renamed to `removed_<acc_no>.csv`, never truly erased
- **Function pointer in nav** — `nav_prompt()` accepts a retry callback so it works for any feature
- **Windows API scanning** — `FindFirstFile` / `FindNextFile` used to glob CSV files at runtime

---

## 🔨 Build

```bash
# Using Makefile
make

# Manual
gcc -Wall -O2 -o banking.exe main.c globals.c security.c utils.c menu.c \
    login.c new_acc.c edit.c transact.c see.c erase.c view_list.c \
    excel.c close_app.c -lm
```

> Requires Windows + GCC (MinGW-w64 or TDM-GCC)

**Files created at runtime:**

| File | Purpose |
|------|---------|
| `credentials.dat` | Hashed login credentials |
| `user_<acc_no>.csv` | Per-account record and transaction log |
| `removed_<acc_no>.csv` | Archived removed accounts |
| `all_customers.csv` | Master list of active accounts |

---

## ✅ Conclusion

This project covers the full lifecycle of a bank account in pure C — registration, login, transactions, updates, and closure — with every change logged to CSV files that can be opened directly in Excel. Security is handled through a custom SHA-256 engine with no external dependencies. The modular file structure keeps each feature clean and independent, making the codebase easy to read, extend, or maintain.
