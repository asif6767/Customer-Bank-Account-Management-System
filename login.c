#include "banking.h"

static void do_login(void);
static void do_register(void);

/* Welcome screen */
void show_login(void)
{
    int choice = -1;

    system("cls");
    system("color 9");
    printf("\n\n\t\t\tCUSTOMER ACCOUNT BANKING MANAGEMENT SYSTEM");
    printf("\n\n\t\t========================================");
    printf("\n\t\t        WELCOME -- PLEASE SIGN IN");
    printf("\n\t\t========================================\n\n");
    printf("\t\t  1. Login to existing account\n");
    printf("\t\t  2. Create a new account\n");
    printf("\n\t\t  Enter your choice: ");

    if (scanf("%d", &choice) != 1)
    {
        while (getchar() != '\n');
        show_login();
        return;
    }

    if (choice == 1)
    {
        do_login();
    }
    else if (choice == 2)
    {
        do_register();
    }
    else
    {
        printf("\n\t\t  Invalid choice! Please enter 1 or 2.\a\n");
        cpu_pause(300000000);
        show_login();
    }
}

/* Credential verification */
static void do_login(void)
{
    char username[60], password[60];
    int  acc_no = 0;

    system("cls");
    system("color 9");
    printf("\n\n\t\t\tCUSTOMER ACCOUNT BANKING MANAGEMENT SYSTEM");
    printf("\n\n\t\t========================================");
    printf("\n\t\t                 LOGIN");
    printf("\n\t\t========================================\n\n");

    printf("\t\tEnter the User ID   : ");
    scanf("%59s", username);

    printf("\t\tEnter the Password  : ");
    scanf("%59s", password);

    /* Hash and verify */
    if (bx_verify_cred(username, password, &acc_no))
        {
        printf("\n\n\t\tLogin successful!  Loading");
        int i;
        for (i = 0; i <= 6; i++)
        {
            cpu_pause(80000000);
            printf(".");
            fflush(stdout);
        }
        system("cls");
        menu();
    }
    else
        {
        printf("\n\n\t\tWrong User ID or Password!\a\a\a");
        login_retry:
        printf("\n\t\tEnter 1 to try again and 0 to exit: ");
        scanf("%d", &main_exit);

        if (main_exit == 1)
        {
            system("cls");
            show_login();
        }

        else if (main_exit == 0)

        {
            system("cls");
            exit_system();
        }
        else
        {
            printf("\nInvalid!");
            cpu_pause(400000000);
            goto login_retry;
        }
    }
}

/* New account registration form */

static void do_register(void)
{
    struct account a;
    char username[60], password[60], conf_pass[60], conf_user[60];
    char fname[40];
    int  acc_no_input;

    memset(&a, 0, sizeof(a));

    system("cls");
    system("color 9");
    printf("\n\n\t\t\tCUSTOMER ACCOUNT BANKING MANAGEMENT SYSTEM");
    printf("\n\n\t\t========================================");
    printf("\n\t\t         CREATE NEW ACCOUNT");
    printf("\n\t\t========================================\n\n");

    printf("\tEnter today's date (mm/dd/yyyy)         : ");
    scanf("%d/%d/%d", &a.deposit.month, &a.deposit.day, &a.deposit.year);

    printf("\tEnter the Office ID number              : ");
    scanf("%19s", a.office_id);

    /* Unique account number check */
    acc_no_entry:
    printf("\tEnter account number                    : ");
    scanf("%d", &acc_no_input);
    csv_filename(acc_no_input, fname);
    {
        FILE *test = fopen(fname, "r");
        if (test)
        {
            fclose(test);
            printf("\n\tAccount number %d is already in use! Try another.\n\n",acc_no_input);
            goto acc_no_entry;
        }
    }
    a.acc_no = acc_no_input;

    printf("\tEnter first name                        : ");
    scanf("%39s", a.first_name);
    //while (getchar() != '\n');
    //fgets(a.first_name, sizeof(a.first_name), stdin);
    //a.first_name[strcspn(a.first_name, "\r\n")] = '\0';

    printf("\tEnter last name                         : ");
    scanf("%39s", a.last_name);
    fgets(a.last_name, sizeof(a.last_name), stdin);
    a.last_name[strcspn(a.last_name, "\r\n")] = '\0';

    snprintf(a.name, sizeof(a.name), "%s %s", a.first_name, a.last_name);

    printf("\tEnter date of birth (mm/dd/yyyy)        : ");
    scanf("%d/%d/%d", &a.dob.month, &a.dob.day, &a.dob.year);

    printf("\tEnter age                               : ");
    scanf("%d", &a.age);

    while (getchar() != '\n');

    printf("\tEnter current address                   : ");
    fgets(a.curr_address, sizeof(a.curr_address), stdin);
    a.curr_address[strcspn(a.curr_address, "\r\n")] = '\0';
    strncpy(a.address, a.curr_address, sizeof(a.address)-1);

    printf("\tEnter permanent address                 : ");
    fgets(a.perm_address, sizeof(a.perm_address), stdin);
    a.perm_address[strcspn(a.perm_address, "\r\n")] = '\0';

    printf("\tEnter citizenship number                : ");
    scanf("%19s", a.citizenship);

    printf("\tEnter phone number                      : ");
    scanf("%lf", &a.phone);

    printf("\tEnter email number                      : ");
    scanf("%lf", &a.email);

    printf("\tEnter amount to deposit ($)             : ");
    scanf("%f", &a.amt);

    printf("\n\tType of account:\n");
    printf("\t  Saving / Current / Fixed1 / Fixed2 / Fixed3\n");
    printf("\tEnter account type                      : ");
    scanf("%11s", a.acc_type);

    printf("\n\t---- Security Setup ----\n");

    /* Username uniqueness check */
    username_entry:
    printf("\tEnter new User ID                       : ");
    scanf("%59s", username);
    printf("\tConfirm User ID                         : ");
    scanf("%59s", conf_user);

    if (strcmp(username, conf_user) != 0)
    {
        printf("\n\tUser IDs do not match! Try again.\a\n\n");
        goto username_entry;
    }
    if (bx_uid_taken(username))
    {
        printf("\n\tThat User ID is already taken! Choose another.\n\n");
        goto username_entry;
    }

    /* Password match check */
    password_entry:
    printf("\tEnter new Password                      : ");
    scanf("%59s", password);
    printf("\tConfirm Password                        : ");
    scanf("%59s", conf_pass);

    if (strcmp(password, conf_pass) != 0)
    {
        printf("\n\tPasswords do not match! Try again.\a\n\n");
        goto password_entry;
    }

    /* Persist account and credentials */
    csv_create_account(&a, fname);

    if (!bx_store_cred(username, password, a.acc_no))
    {
        printf("\n[ERROR] Could not save credentials. Account creation failed!\n");
        return;
    }

    csv_rebuild_master();

    printf("\n\n\t============================================\n");
    printf("\t  Account Created Successfully!\n");
    printf("\t  Account Number : %d\n", a.acc_no);
    printf("\t  Full Name      : %s %s\n", a.first_name, a.last_name);
    printf("\t  Data saved to  : %s\n", fname);
    printf("\t============================================\n");

    nav_simple();
}

/* Public registration wrapper */
void register_account(void)
{
    do_register();
}
