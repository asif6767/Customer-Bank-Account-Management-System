#ifndef BANKING_H
#define BANKING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/* Date structure */
struct date
{
    int month, day, year;
};

/* Account record */
struct account
{
    char   name[60];
    char   first_name[40];

    char   last_name[40];
    char   office_id[20];
    int    acc_no;
    int    age;
    char   address[80];
    char   email[60];
    char   curr_address[80];
    char   perm_address[80];
    char   citizenship[20];
    double phone;
    char   acc_type[12];
    float  amt;
    struct date dob;
    struct date deposit;
    struct date withdraw;
};

/* Shared globals */
extern int main_exit;
extern struct account add, upd, check, rem, transaction;

/* security.c */
void   bx_digest_hex(const char *src, char *hex_out);
int    bx_store_cred(const char *uid, const char *pwd, int acc_no);
int    bx_verify_cred(const char *uid, const char *pwd, int *acc_out);
int    bx_uid_taken(const char *uid);

/* utils.c */
float  calc_interest(float t, float amount, int rate);
void   cpu_pause(int j);
void   nav_prompt(int allow_retry, void (*retry_fn)(void));
void   nav_simple(void);

/* login.c */
void   show_login(void);
void   register_account(void);

/* menu.c */
void   menu(void);

/* new_acc.c */
void   open_account(void);

/* edit.c */
void   update_account(void);

/* transact.c */
void   do_transaction(void);

/* see.c */
void   view_account(void);

/* erase.c */
void   remove_account(void);

/* view_list.c */
void   list_accounts(void);

/* excel.c */
void   csv_create_account(const struct account *a, const char *fname);

void   csv_append_update(const char *fname, const char *field,const char *old_val, const char *new_val,const char *date_str);

void   csv_append_txn(const char *fname, const char *type,float amount, float balance,const char *date_str);

void   csv_mark_removed(const char *fname, const char *hx,const char *date_str);

void   csv_rebuild_master(void);

void   csv_filename(int acc_no, char *out);

/* close_app.c */
void   exit_system(void);

#endif
