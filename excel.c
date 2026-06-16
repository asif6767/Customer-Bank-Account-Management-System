#include "banking.h"

/* Build per-user filename */
void csv_filename(int acc_no, char *out)
{
    sprintf(out, "user_%d.csv", acc_no);
}

/* Create new account CSV */
void csv_create_account(const struct account *a, const char *fname)
{
    FILE *fp = fopen(fname, "w");
    if (!fp)
        {
        printf("\n[ERROR] Could not create file: %s\n", fname);
        return;
    }

    /* Column headers */
    fprintf(fp,
        "Record Type,Account No,Office ID,First Name,Last Name,"
        "Date of Birth,Age,Current Address,Permanent Address,"
        "Citizenship No,Phone,Account Type,Balance,Open Date,Status\n");

    /* Account info row */
    fprintf(fp,
        "ACCOUNT_INFO,%d,%s,%s,%s,%02d/%02d/%04d,%d,"
        "\"%s\",\"%s\",%s,%.0lf,%s,%.2f,%02d/%02d/%04d,ACTIVE\n",
        a->acc_no,
        a->office_id,
        a->first_name,
        a->last_name,
        a->dob.month, a->dob.day, a->dob.year,
        a->age,
        a->curr_address,
        a->perm_address,
        a->citizenship,
        a->phone,
        a->acc_type,
        a->amt,
        a->deposit.month, a->deposit.day, a->deposit.year);

    /* Transaction log header */
    fprintf(fp, "\n");
    fprintf(fp, "Record Type,Date,Transaction Type,Amount,Balance,Notes\n");

    fclose(fp);
}

/* Append field update history row */
void csv_append_update(const char *fname, const char *field,const char *old_val, const char *new_val,const char *date_str)
{
    FILE *fp = fopen(fname, "a");
    if (!fp)
    {
        return;
    }
    fprintf(fp, "UPDATE,%s,\"%s (OLD)\",\"%s (NEW)\",%s\n",field, old_val, new_val, date_str);
    fclose(fp);
}

/* Append deposit/withdrawal row */
void csv_append_txn(const char *fname, const char *type,float amount, float balance,const char *date_str)
{
    FILE *fp = fopen(fname, "a");
    if (!fp)
    {
        return;
    }
    fprintf(fp, "TRANSACTION,%s,%s,%.2f,%.2f,\n",date_str, type, amount, balance);
    fclose(fp);
}

/* Append soft-delete audit row */
void csv_mark_removed(const char *fname, const char *hx,const char *date_str)
{
    FILE *fp = fopen(fname, "a");
    if (!fp)
    {
        return;
    }
    fprintf(fp,
        "\nREMOVED,%s,Account removed on %s,"
        "Username hash (audit): %s\n",date_str, date_str, hx);
    fclose(fp);
}

/* Rebuild master customer list */
void csv_rebuild_master(void)
{
    FILE *master = fopen("all_customers.csv", "w");
    if (!master)
    {
        return;
    }

    fprintf(master,
        "Account No,Office ID,First Name,Last Name,"
        "Date of Birth,Age,Current Address,Permanent Address,"
        "Citizenship No,Phone,Account Type,Balance,Open Date\n");

    /* Scan all per-user CSV files */
    WIN32_FIND_DATA fd;
    HANDLE h = FindFirstFile("user_*.csv", &fd);
    if (h == INVALID_HANDLE_VALUE)
        {
        fprintf(master, "(No active accounts)\n");
        fclose(master);
        return;
    }

    int count = 0;
    do
        {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
        }

        FILE *uf = fopen(fd.cFileName, "r");
        if (!uf)
        {
            continue;
        }

        char line[512], info[512] = "";
        int  removed = 0;

        while (fgets(line, sizeof(line), uf))
            {
            if (strncmp(line, "ACCOUNT_INFO,", 13) == 0)
                {
                    strncpy(info, line, sizeof(info)-1);
                }
            if (strncmp(line, "REMOVED,", 8) == 0)
                {
                    removed = 1;
                }
        }
        fclose(uf);

        if (!removed && info[0])
            {
            /* Strip prefix and ACTIVE suffix */
            char *data = info + 13;
            data[strcspn(data, "\r\n")] = '\0';
            char *active = strstr(data, ",ACTIVE");
            if (active) *active = '\0';
            fprintf(master, "%s\n", data);
            count++;
        }
    }
    while (FindNextFile(h, &fd));

    FindClose(h);
    if (count == 0)
        fprintf(master, "(No active accounts)\n");
    fclose(master);
}
