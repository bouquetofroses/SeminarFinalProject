#include "project.h"

int test_case_count = 0;
int test_fail_count = 0;

void print_test_result(const char *desc, int passed) {
    test_case_count++;
    if (passed) {
        printf("[PASS] %s\n", desc);
    } else {
        printf("[FAIL] %s\n", desc);
        test_fail_count++;
    }
}


void backup_csv_full() {
    FILE *src = fopen(CSV_FILE, "r");
    if (!src) return;
    FILE *dst = fopen("Seminar_backup_full.csv", "w");
    if (!dst) { fclose(src); return; }
    char buf[1024];
    while (fgets(buf, sizeof(buf), src)) fputs(buf, dst);
    fclose(src); fclose(dst);
}

void restore_csv_full() {
    FILE *src = fopen("Seminar_backup_full.csv", "r");
    if (!src) return;
    FILE *dst = fopen(CSV_FILE, "w");
    if (!dst) { fclose(src); return; }
    char buf[1024];
    while (fgets(buf, sizeof(buf), src)) fputs(buf, dst);
    fclose(src); fclose(dst);
    remove("Seminar_backup_full.csv");
}

void setup_test_csv_full() {
    FILE *fp = fopen(CSV_FILE, "w");
    if (!fp) return;
    fprintf(fp, "ID,ParticipantName,Email,PhoneNumber,RegistrationDate,Status\n");
    fprintf(fp, "P0001,Alice,alice@test.com,0812345678,2025-10-01,Active\n");
    fprintf(fp, "P0002,Bob,bob@test.com,0822222222,2025-10-02,Inactive\n");
    fprintf(fp, "P0003,Charlie,charlie@test.com,0833333333,2025-10-03,Active\n");
    fclose(fp);
}

int simulate_update_record(const char *targetEmail, const char *newEmail, const char *newPhone, const char *newName) {
    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) return 0;
    FILE *tmp = fopen("temp_update.csv", "w");
    if (!tmp) { fclose(fp); return 0; }
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); fputs(line, tmp);
    int updated = 0;
    while (fgets(line, sizeof(line), fp)) {
        char id[64], name[256], email[256], phone[64], date[64], status[64];
        if (!parse_line(line, id, name, email, phone, date, status)) {
            fputs(line, tmp);
            continue;
        }
        if (strcmp(email, targetEmail) == 0) {
            if (newEmail && strlen(newEmail) > 0) strcpy(email, newEmail);
            if (newPhone && strlen(newPhone) > 0) strcpy(phone, newPhone);
            if (newName && strlen(newName) > 0) strcpy(name, newName);
            fprintf(tmp, "%s,%s,%s,%s,%s,%s\n", id, name, email, phone, date, status);
            updated = 1;
        } else fputs(line, tmp);
    }
    fclose(fp); fclose(tmp);
    remove(CSV_FILE); rename("temp_update.csv", CSV_FILE);
    return updated;
}

int participant_edit_self_sim(const char *email, const char *phone,
    const char *newName, const char *newEmail, const char *newPhone) {
    
    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) return 0;
    FILE *temp = fopen("temp_selfedit.csv", "w");
    if (!temp) { fclose(fp); return 0; }

    char line[MAXLINE];
    fgets(line, sizeof(line), fp); fputs(line, temp);
    int found = 0, updated = 0;

    while (fgets(line, sizeof(line), fp)) {
        char id[64], name[256], e[256], p[64], date[64], status[64];
        if (!parse_line(line, id, name, e, p, date, status)) {
            fputs(line, temp);
            continue;
        }

        if (strcmp(e, email) == 0 && strcmp(p, phone) == 0) {
            found = 1;

            if (newEmail && strlen(newEmail) > 0) {
                if (!validateEmail((char *)newEmail)) {
                    fclose(fp); fclose(temp); remove("temp_selfedit.csv");
                    return -1;
                }
                if (isEmailDuplicateExcept(newEmail, id)) {
                    fclose(fp); fclose(temp); remove("temp_selfedit.csv");
                    return -2;
                }
                strcpy(e, newEmail);
            }

            if (newPhone && strlen(newPhone) > 0) {
                if (!validatePhone((char *)newPhone)) {
                    fclose(fp); fclose(temp); remove("temp_selfedit.csv");
                    return -3;
                }
                if (isPhoneDuplicateExcept(newPhone, id)) {
                    fclose(fp); fclose(temp); remove("temp_selfedit.csv");
                    return -4;
                }
                strcpy(p, newPhone);
            }

            if (newName && strlen(newName) > 0)
                strcpy(name, newName);

            fprintf(temp, "%s,%s,%s,%s,%s,%s\n", id, name, e, p, date, status);
            updated = 1;
        } else fputs(line, temp);
    }

    fclose(fp); fclose(temp);

    if (!found) { remove("temp_selfedit.csv"); return 0; }
    remove(CSV_FILE); rename("temp_selfedit.csv", CSV_FILE);
    return updated ? 1 : 0;
}
int check_record_status(const char *id_to_find) {
    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) return 0;
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); // skip header
    while (fgets(line, sizeof(line), fp)) {
        char id[64], name[256], email[256], phone[64], date[64], status[64];
        if (parse_line(line, id, name, email, phone, date, status)) {
            if (strcmp(id, id_to_find) == 0) {
                fclose(fp);
                if (strcasecmp(status, "Active") == 0) return 1;
                if (strcasecmp(status, "Inactive") == 0) return 2;
                return 0; 
            }
        }
    }
    fclose(fp);
    return 0;
}

int simulate_add_record(const char *name, const char *email, const char *phone, char *out_id, size_t out_sz) {
    char regDate[64], id[64];
    getToday(regDate, sizeof(regDate));
    generateID(id, sizeof(id));
    if (saveCSV(id, name, email, phone, regDate, "Active")) {
        strncpy(out_id, id, out_sz);
        return 1;
    }
    return 0;
}

int simulate_delete_record(const char *targetID) {
    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) return 0;
    FILE *tmp = fopen("temp_delete.csv", "w");
    if (!tmp) { fclose(fp); return 0; }
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); fputs(line, tmp);
    int deleted = 0;
    while (fgets(line, sizeof(line), fp)) {
        char id[64], name[256], email[256], phone[64], date[64], status[64];
        if (!parse_line(line, id, name, email, phone, date, status)) {
            fputs(line, tmp);
            continue;
        }
        if (strcmp(id, targetID) == 0) {
            fprintf(tmp, "%s,%s,%s,%s,%s,%s\n", id, name, email, phone, date, "Inactive");
            deleted = 1;
        } else fputs(line, tmp);
    }
    fclose(fp); fclose(tmp);
    if (deleted) {
        remove(CSV_FILE); rename("temp_delete.csv", CSV_FILE);
    } else {
        remove("temp_delete.csv");
    }
    return deleted;
}
