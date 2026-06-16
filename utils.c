#include "banking.h"

/* Simple interest formula */
float calc_interest(float t, float amount, int rate)
{
    return (rate * t * amount) / 100.0f;
}

/* CPU busy-wait */
void cpu_pause(int j)
{
    int i, k;
    for (i = 0; i < j; i++)
        k = i;
    (void)k;
}

/* Menu or exit prompt */
void nav_simple(void)
{
    nav_again:
    printf("\n\n\t\tEnter 1 to go to the main menu and 0 to exit: ");
    scanf("%d", &main_exit);
    system("cls");
    if (main_exit == 1)
        {
        menu();
    }
    else if (main_exit == 0)
        {
        exit_system();
    }
    else
        {
        printf("\nInvalid!\a");
        goto nav_again;
    }
}

/* Retry, menu, or exit prompt */
void nav_prompt(int allow_retry, void (*retry_fn)(void))
{
    nav_p_again:
    if (allow_retry)
        {
            printf("\nEnter 0 to try again, 1 to return to main menu and 2 to exit: ");
        }
    else
        {
            printf("\nEnter 1 to return to main menu and 2 to exit: ");
        }

    scanf("%d", &main_exit);
    system("cls");

    if (main_exit == 0 && allow_retry && retry_fn != NULL)
        {
        retry_fn();
    }
    else if (main_exit == 1)
        {
        menu();
    }
    else if (main_exit == 2)
        {
        exit_system();
    }
    else
        {
        printf("\nInvalid!\a");
        goto nav_p_again;
    }
}
