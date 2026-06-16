#include "banking.h"

void menu(void)
{
    int choice;

    system("cls");
    system("color 9");
    printf("\n\n\t\t\tCUSTOMER ACCOUNT BANKING MANAGEMENT SYSTEM");
    printf("\n\n\n\t\t\t======= WELCOME TO THE MAIN MENU =======\n\n");
    printf("\t\t1. Create new account\n");
    printf("\t\t2. Update information of existing account\n");
    printf("\t\t3. For transactions\n");
    printf("\t\t4. Check the details of existing account\n");
    printf("\t\t5. Removing existing account\n");
    printf("\t\t6. View customer's list\n");
    printf("\t\t7. Exit\n\n\n");
    printf("\t\t Enter your choice: ");
    scanf("%d", &choice);

    system("cls");

    switch (choice)
    {
        case 1: open_account();
        break;
        case 2: update_account();
        break;
        case 3: do_transaction();
        break;
        case 4: view_account();
        break;
        case 5: remove_account();
        break;
        case 6: list_accounts();
        break;
        case 7: exit_system();
        break;

        default:
            printf("\nInvalid choice! Please enter 1-7.\a\n");
            cpu_pause(300000000);
            menu();
            break;
    }
}
