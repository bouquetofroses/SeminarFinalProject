#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<assert.h>
#include<time.h>

#define CSV_FILE "Seminar.csv"
#define MAXLINE 1024

int createfile();
int readCSV();
int saveCSV(const char *id, const char *name, const char *email, const char *phone, const char *regDate, const char *status);
int parse_line(const char *line, char *id, char *name, char *email, char *phone, char *date, char *status);
void trim_newline(char *s);
int generateID(char *out_id, size_t out_sz);

int validateEmail(char *email);
int isEmailDuplicateExcept(const char *email, const char *except_id);
int isEmailDuplicate(const char *email);

int validatePhone(char *phone);

int validateDate(int y, int m, int d);
char *getToday(char *buf, size_t bufsz);

int admin_login();
int admin_menu();
void display_all_admin();
int search_controller(int isAdmin);
int update_direct_admin();
int delete_direct_admin();

int participant_menu();
void display_limited();
int add_participant_interactive();
int participant_edit_self();
int participant_delete_self();

void print_test_result(const char *desc, int passed);
void run_full_unit_test_suite();

int check_record_status(const char *id_to_find);
int simulate_add_record(const char *name, const char *email, const char *phone, char *out_id, size_t out_sz);
int simulate_delete_record(const char *targetID);
void run_full_e2e_test_suite();

//สร้างไฟล์
int createfile()
{
    FILE *fp = fopen("Seminar.csv", "r");
    if (fp == NULL) {  
        fp = fopen("Seminar.csv", "w");
        if (fp == NULL) {
            printf("Cannot create file\n");
            return 0;
        }
        fprintf(fp, "ID,ParticipantName,Email,PhoneNumber,RegistrationDate,Status\n");
        fclose(fp);
    } else {
        fclose(fp);
    }
    return 1;
}

//อ่านข้อมูลผู้เข้าร่วมสัมมนาจากไฟล์ CSV
int readCSV()
{
    FILE *fp = fopen("Seminar.csv","r");
    if (fp == NULL)
    {
        printf("File: %p\n",fp);
        printf("Cannot open the file\n");
    }
    fclose (fp);
    return 1;
}

//บันทึกข้อมูลผู้เข้าร่วมสัมมนาลงไฟล์ CSV
int saveCSV(const char *id, const char *name, const char *email, const char *phone, const char *regDate, const char *status)
{
    FILE *fp;
    fp = fopen("Seminar.csv","a");
    if (fp==NULL)
    {
        printf("Cannot open file\n");
        return 0;
    }
    fprintf(fp,"%s,%s,%s,%s,%s,%s\n", id, name, email, phone, regDate, status);
    fclose (fp);
    return 1;
}

//ตรวจสอบอีเมล
int validateEmail (char *email)
{
    int len = strlen(email);
    int atCount = 0;
    const char *atPos = NULL;

    for (int i = 0; i < len; i++) {
        if (isspace(email[i])) return 0;  // ห้าม space
        if (email[i] == '@') {
            atCount++;
            atPos = &email[i];
        }
    }

    if (atCount != 1) return 0;                   // ต้องมี @ เพียงตัวเดียว
    if (email[0] == '@' || email[len-1] == '@') return 0; // ห้ามขึ้นต้น/ท้าย @
    if (strchr(atPos, '.') == NULL) return 0;    // ต้องมี . หลัง @
    return 1 ;
}



int isEmailDuplicateExcept(const char *email, const char *except_id) {
    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) return 0;
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); // skip header
    while (fgets(line, sizeof(line), fp)) {
        char id[64], name[256], e[256], phone[64], date[64], status[64];
        if (parse_line(line, id, name, e, phone, date, status)) {
            // ตรวจเฉพาะอีเมลที่เหมือนกัน และมีสถานะ Active
            if (strcasecmp(e, email) == 0 && strcasecmp(status, "Active") == 0) {
                if (except_id == NULL) { fclose(fp); return 1; } // email ซ้ำกับ Active record อื่น
                if (strcmp(id, except_id) != 0) { fclose(fp); return 1; }
            }
        }
    }
    fclose(fp);
    return 0;
}

int isEmailDuplicate(const char *email) {
    return isEmailDuplicateExcept(email, NULL);
}

int isPhoneDuplicateExcept(const char *phone, const char *except_id) {
    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) return 0;
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); // skip header
    while (fgets(line, sizeof(line), fp)) {
        char id[64], name[256], email[256], p[64], date[64], status[64];
        if (parse_line(line, id, name, email, p, date, status)) {
            // ตรวจเฉพาะเบอร์ที่เหมือนกัน และมีสถานะ Active
            if (strcmp(p, phone) == 0 && strcasecmp(status, "Active") == 0) {
                if (except_id == NULL) { fclose(fp); return 1; } // เบอร์ซ้ำกับ Active record อื่น
                if (strcmp(id, except_id) != 0) { fclose(fp); return 1; }
            }
        }
    }
    fclose(fp);
    return 0;
}

int isPhoneDuplicate(const char *phone) {
    return isPhoneDuplicateExcept(phone, NULL);
}

//ตรวจสอบเบอร์
int validatePhone(char *phone) {
    if (strlen(phone) != 10) return 0;
    for (int i = 0; i < 10; i++) {
        if (!isdigit(phone[i])) return 0;
    }
    return (strncmp(phone, "09", 2) == 0 ||
            strncmp(phone, "08", 2) == 0 ||
            strncmp(phone, "06", 2) == 0);
}

// ตรวจสอบว่าเป็น leap year หรือไม่
int isLeapYear(int year) {
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

// ตรวจสอบความถูกต้องของวันที่
int validateDate(int y, int m, int d) {
    if (y < 1900 || y > 2100) return 0;
    if (m < 1 || m > 12) return 0;

    int daysInMonth[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (isLeapYear(y)) daysInMonth[2] = 29;

    if (d < 1 || d > daysInMonth[m]) return 0;

    return 1;
}

// คืนค่าวันที่ปัจจุบันเป็น "YYYY-MM-DD"
char *getToday(char *buf, size_t bufsz) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buf, bufsz, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    return buf;
}

// ตัด \n ปลายสตริง
void trim_newline(char *s) {
    if (!s) return;
    size_t L = strlen(s);
    if (L==0) return;
    if (s[L-1]=='\n') s[L-1]=0;
    if (L>1 && s[L-2]=='\r') s[L-2]=0;
}

int parse_line(const char *line, char *id, char *name, char *email, char *phone, char *date, char *status) {
    if (!line) return 0;
    char tmp[MAXLINE];
    strncpy(tmp, line, sizeof(tmp)-1); tmp[sizeof(tmp)-1]=0;
    trim_newline(tmp);

    int commas = 0;
    for (size_t i=0;i<strlen(tmp);i++) if (tmp[i]==',') commas++;

    if (commas == 3) {
        char *t = tmp;
        char *f1 = strtok(t, ",");
        char *f2 = strtok(NULL, ",");
        char *f3 = strtok(NULL, ",");
        char *f4 = strtok(NULL, ",");
        if (!f1 || !f2 || !f3 || !f4) return 0;
        strcpy(id, "-");
        strcpy(name, f1);
        strcpy(email, f2);
        strcpy(phone, f3);
        strcpy(date, f4);
        strcpy(status, "Active");
        return 1;
    } else {
        char *t = tmp;
        char *token;
        token = strtok(t, ","); if (!token) return 0; strcpy(id, token);
        token = strtok(NULL, ","); if (!token) return 0; strcpy(name, token);
        token = strtok(NULL, ","); if (!token) return 0; strcpy(email, token);
        token = strtok(NULL, ","); if (!token) return 0; strcpy(phone, token);
        token = strtok(NULL, ","); if (!token) return 0; strcpy(date, token);
        token = strtok(NULL, ",");
        if (!token) strcpy(status, "Active"); else strcpy(status, token);
        return 1;
    }
}

// สร้าง ID 
int generateID(char *out_id, size_t out_sz) {
    FILE *fp = fopen("Seminar.csv", "r");
    int maxnum = 0 ;
    if (!fp) {
        snprintf(out_id, out_sz, "P%04d", 1);
        return 1;
    }
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); // skip header
    while (fgets(line, sizeof(line), fp)) {
        char id[64], name[256], email[256], phone[64], date[64], status[64];
        if (parse_line(line, id, name, email, phone, date, status)) {
            if (id[0]=='P') {
                int num = atoi(id+1);
                if (num > maxnum) maxnum = num;
            }
        }
    }
    fclose(fp);
    int next = maxnum + 1;
    snprintf(out_id, out_sz, "P%04d", next);
    return 1;
}

int compareByID(const void *a, const void *b) {
    const char (*recA)[MAXLINE] = a;
    const char (*recB)[MAXLINE] = b;
    char idA[64], nameA[256], emailA[256], phoneA[64], dateA[64], statusA[64];
    char idB[64], nameB[256], emailB[256], phoneB[64], dateB[64], statusB[64];
    parse_line(*recA, idA, nameA, emailA, phoneA, dateA, statusA);
    parse_line(*recB, idB, nameB, emailB, phoneB, dateB, statusB);
    return strcmp(idA, idB);
}

int admin_login() {
    char user[64], pass[64];
    printf("\nAdmin login required.\n");
    printf ("========================\n");
    printf("|   USERNAME = admin   |\n");
    printf("|   PASSWORD = 1234    |\n");
    printf ("========================\n");
    printf("Username: ");
    if (scanf("%49s", user) != 1) { while(getchar()!='\n'); return 0; }
    printf("Password: ");
    if (scanf("%49s", pass) != 1) { while(getchar()!='\n'); return 0; }
    while(getchar()!='\n');

    if (strcmp(user, "admin") == 0 && strcmp(pass, "1234") == 0) {
        printf("\nLogin successful.\n");
        return 1;
    } else {
        printf("Invalid credentials.\n");
        return 0;
    }
}

//เพิ่มข้อมูลผู้เข้าร่วมใหม่
int add_participant_interactive() {
    char name[256], email[256], phone[64], regDate[64], id[64];
    while (1) {
        printf("Enter participant name: ");
        if (fgets(name, sizeof(name), stdin) == NULL) return 0;
        trim_newline(name);
        if (strlen(name) == 0) { printf("Name cannot be empty.\n"); continue; }

        // email
        do {
            printf("Enter email: ");
    
            if (fgets(email, sizeof(email), stdin) == NULL) return 0;
            trim_newline(email);
            for (int i = 0; email[i]; i++)
                 email[i] = tolower((unsigned char)email[i]);
            if (!validateEmail(email)) { printf("Invalid email format.\n"); continue; }

            if (isEmailDuplicate(email)) { 
                printf("This email has already been registered by an active participant.\n"); 
                continue; 
            }
            break;
        } while (1);

        // phone
        do {
            printf("Enter phone (10 digits, starts 09/08/06): ");
            if (fgets(phone, sizeof(phone), stdin) == NULL) return 0;
            trim_newline(phone);
            if (!validatePhone(phone)) { printf("Invalid phone.\n"); continue; }
            if (isPhoneDuplicate(phone)) {
                printf("This phone number has already been registered by an active participant.\n");
                continue; 
            }
            break;
        } while (1);

        // regDate auto
        getToday(regDate, sizeof(regDate));

        // show summary and confirm
        printf("\nPlease confirm information:\n");
        printf("Name: %s\nEmail: %s\nPhone: %s\nRegistration Date: %s\n", name, email, phone, regDate);
        char c;
        printf("Save this participant? (y/n): ");
        if (scanf(" %c", &c) != 1) { while(getchar()!='\n'); return 0; }
        while(getchar()!='\n');
        if (c == 'y' || c == 'Y') {
            generateID(id, sizeof(id));
            if (saveCSV(id, name, email, phone, regDate, "Active")) {
                printf("Saved. ID: %s\n", id);
                return 1;
            } else {
                printf("Failed to save.\n");
                return 0;
            }
        } else {
            printf("Let's re-enter information.\n");
            continue;
        }
    }
    return 0;
}

int update_info_interactive(FILE *temp, const char *orig_id, const char *orig_status) {
    
    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) { printf("Cannot open file.\n"); return 0; }
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); // header
    char currName[256] = "", currEmail[256] = "", currPhone[64] = "", currDate[64] = "";
    while (fgets(line, sizeof(line), fp)) {
        char id[64], name[256], email[256], phone[64], date[64], status[64];
        if (parse_line(line, id, name, email, phone, date, status)) {
            if (strcmp(id, orig_id) == 0) {
                strcpy(currName, name); strcpy(currEmail, email); strcpy(currPhone, phone); strcpy(currDate, date);
                break;
            }
        }
    }
    fclose(fp);

    char newName[256], newEmail[256], newPhone[64], newDate[64];
    int confirmed = 0;
    while (!confirmed) {
        printf("\nUpdating ID: %s (Press Enter to keep current value)\n", orig_id);
        printf("Current Name: %s\nNew Name: ", currName);
        if (fgets(newName, sizeof(newName), stdin) == NULL) return 0;
        trim_newline(newName);
        if (strlen(newName) == 0) strcpy(newName, currName);

        // email with duplication check (except self)
        while (1) {
            printf("Current Email: %s\nNew Email: ", currEmail);
            if (fgets(newEmail, sizeof(newEmail), stdin) == NULL) return 0;
            trim_newline(newEmail);
            if (strlen(newEmail) == 0) { strcpy(newEmail, currEmail); break; }
            if (!validateEmail(newEmail)) { printf("Invalid email format.\n"); continue; }
            
            // การตรวจสอบอีเมล: ถูกต้องแล้ว (ใช้ continue; เพื่อกลับไปป้อนใหม่)
            if (isEmailDuplicateExcept(newEmail, orig_id)) { 
                printf("This email is used by another participant.\n"); 
                continue; 
            }
            break;
        }


        // phone
        while (1) {
            printf("Current Phone: %s\nNew Phone: ", currPhone);
            if (fgets(newPhone, sizeof(newPhone), stdin) == NULL) return 0;
            trim_newline(newPhone);
            if (strlen(newPhone) == 0) { strcpy(newPhone, currPhone); break; }
            if (!validatePhone(newPhone)) { printf("Invalid phone.\n"); continue; }
            if (isPhoneDuplicateExcept(newPhone, orig_id)) {
                printf("This phone number is used by another participant.\n"); 
                continue; // ถ้าซ้ำ ให้กลับไปป้อนใหม่
            }
            break;
        }
        

        // date
        printf("Current RegDate: %s\nNew Date (YYYY-MM-DD or 0 for today): ", currDate);
        if (fgets(newDate, sizeof(newDate), stdin) == NULL) return 0;
        trim_newline(newDate);
        if (strlen(newDate) == 0) strcpy(newDate, currDate);
        else if (strcmp(newDate, "0") == 0) getToday(newDate, sizeof(newDate));
        else {
            int y,m,d;
            if (sscanf(newDate, "%d-%d-%d", &y, &m, &d) != 3 || !validateDate(y,m,d)) {
                printf("Invalid date format. Using old date.\n");
                strcpy(newDate, currDate);
            }
        }

        // summary
        printf("\nPlease confirm updated info:\nID: %s\nName: %s\nEmail: %s\nPhone: %s\nRegDate: %s\nStatus: %s\n", orig_id, newName, newEmail, newPhone, newDate, orig_status);
        char c;
        printf("\nIs this correct? (y/n): ");
        if (scanf(" %c", &c) != 1) { while(getchar()!='\n'); return 0; }
        while(getchar()!='\n');
        if (c == 'y' || c == 'Y') {
            fprintf(temp, "%s,%s,%s,%s,%s,%s\n", orig_id, newName, newEmail, newPhone, newDate, orig_status);
            printf("Record updated.\n");
            confirmed = 1;
        } else {
            printf("Re-enter information.\n");
        }
    }
    return 1;
}

// เปลี่ยนสถานะเป็น Inactive 
int set_inactive(FILE *temp, const char *line)
{
    char id[50], name[200], email[200], phone[50], date[50], status[20];
    if (!parse_line(line, id, name, email, phone, date, status)) {
        // ถ้า parse ไม่ได้ ให้เขียน original ลงไป
        fprintf(temp, "%s", line);
        return 0;
    }

    char confirm;
    printf("Confirm delete (set to Inactive) for ID %s, Name: %s ? (y/n): ", id, name);
    if (scanf(" %c", &confirm) != 1) { while(getchar()!='\n'); fprintf(temp, "%s", line); return 0; }
    while(getchar()!='\n');
    if (confirm != 'y' && confirm != 'Y') {
        // ยกเลิก 
        fprintf(temp, "%s", line);
        printf("Delete cancelled.\n");
        return 0;
    }

    // เขียนข้อมูลเดิมแต่เปลี่ยน Status
    fprintf(temp, "%s,%s,%s,%s,%s,%s\n", id, name, email, phone, date, "Inactive");
    printf("Record (ID %s) set to Inactive.\n", id);
    return 1;
}

//ค้นหาข้อมูลผู้เข้าร่วมสัมมนา
int search_controller(int isAdmin) {
    char keyword[256];
    printf("Enter search keyword (Name/Email/Phone/Date/ID): ");
    if (fgets(keyword, sizeof(keyword), stdin) == NULL) return 0;
    trim_newline(keyword);
    if (strlen(keyword) == 0) { printf("Empty keyword.\n"); return 0; }

    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) { printf("Cannot open %s\n", CSV_FILE); return 0; }
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); // skip header

    char records[200][MAXLINE];
    int count = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, keyword)) {
            strcpy(records[count++], line);
        }
    }
    fclose(fp);

    if (count == 0) { printf("No record found for '%s'\n", keyword); return 0; }

    // Sort by ID
    qsort(records, count, sizeof(records[0]), compareByID);

    if (isAdmin) {
    printf("\n--- Found %d record(s) ---\n", count);
    for (int i = 0; i < count; i++) {
        char id[64], name[256], email[256], phone[64], date[64], status[64];
        parse_line(records[i], id, name, email, phone, date, status);
        printf("\n[%d] Participant Information\n", i+1);
        printf("ID: %s\nName: %s\nEmail: %s\nPhone: %s\nRegistration Date: %s\nStatus: %s\n",
               id, name, email, phone, date, status);
        }
    } else {
        printf("\n===================================================================\n");
        printf("| %-6s | %-28s | %-12s | %-8s |\n", "ID", "Name", "Reg. Date", "Status");
        printf("===================================================================\n");
        for (int i = 0; i < count; i++) {
            char id[64], name[256], email[256], phone[64], date[64], status[64];
            parse_line(records[i], id, name, email, phone, date, status);
            printf("| %-6s | %-28s | %-12s | %-8s |\n", id, name, date, status);
        }
        printf("===================================================================\n");
    }

    // สำหรับ admin ให้เลือกจัดการ record
    if (isAdmin) {
        int idx;
        printf("\nSelect index to manage (0 = skip): ");
        if (scanf("%d", &idx) != 1) { while(getchar()!='\n'); return 0; }
        while(getchar()!='\n');
        if (idx <= 0 || idx > count) { printf("Skipped.\n"); return 0; }

        char id[64], name[256], email[256], phone[64], date[64], status[64];
        parse_line(records[idx-1], id, name, email, phone, date, status);

        char action;
        printf("Choose action: (e)dit / (d)elete / (s)kip: ");
        if (scanf(" %c", &action) != 1) { while(getchar()!='\n'); return 0; }
        while(getchar()!='\n');

        if (action=='e' || action=='E') update_direct_admin();
        else if (action=='d' || action=='D') delete_direct_admin();
        else printf("Skipped.\n");
    }

    return 1;
}

//update admin only
int update_direct_admin() {
    
    char keyword[256];
    printf("Enter keyword to find record to update: ");
    if (fgets(keyword, sizeof(keyword), stdin) == NULL) return 0;
    trim_newline(keyword);
    if (strlen(keyword) == 0) { printf("Empty keyword.\n"); return 0; }

    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) { printf("Cannot open file.\n"); return 0; }
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); 
    char records[200][MAXLINE];
    int count = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, keyword)) {
            strcpy(records[count++], line);
        }
    }
    fclose(fp);

    if (count == 0) { printf("No matches found.\n"); return 0; }

    printf("\nFound %d record(s):\n", count);
    for (int i=0;i<count;i++) {
        char id[64], name[256], email[256], phone[64], date[64], status[64];
        parse_line(records[i], id, name, email, phone, date, status);
        printf("[%d] ID:%s Name:%s Email:%s Phone:%s RegDate:%s Status:%s\n", i+1, id, name, email, phone, date, status);
    }
    int idx;
    printf("Select index to update (0 = cancel): ");
    if (scanf("%d", &idx) != 1) { while(getchar()!='\n'); return 0; }
    while(getchar()!='\n');
    if (idx <= 0 || idx > count) { printf("Cancelled.\n"); return 0; }
    // temp
    FILE *temp = fopen("temp.csv", "w");
    if (!temp) { printf("Cannot create temp file.\n"); return 0; }
    fprintf(temp, "ID,ParticipantName,Email,PhoneNumber,RegistrationDate,Status\n");
    for (int i=0;i<count;i++) {
        if (i == idx-1) {
            char id[64], name[256], email[256], phone[64], date[64], status[64];
            parse_line(records[i], id, name, email, phone, date, status);
            update_info_interactive(temp, id, status);
        } else {
            fprintf(temp, "%s", records[i]);
        }
    }
    
    FILE *orig = fopen(CSV_FILE, "r");
    if (!orig) { fclose(temp); printf("Cannot reopen original file.\n"); return 0; }
    fgets(line, sizeof(line), orig);
    while (fgets(line, sizeof(line), orig)) {
        int skip = 0;
        for (int i=0;i<count;i++) if (strcmp(line, records[i]) == 0) { skip = 1; break; }
        if (!skip) fprintf(temp, "%s", line);
    }
    fclose(orig); fclose(temp);
    remove(CSV_FILE); rename("temp.csv", CSV_FILE);
    printf("Update completed.\n");
    return 1;
}

//delete admin only (inactive)
int delete_direct_admin() {
    char keyword[256];
    printf("Enter keyword to find record to delete (set Inactive): ");
    if (fgets(keyword, sizeof(keyword), stdin) == NULL) return 0;
    trim_newline(keyword);
    if (strlen(keyword) == 0) { printf("Empty keyword.\n"); return 0; }

    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) { printf("Cannot open file.\n"); return 0; }
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); 
    char records[200][MAXLINE];
    int count = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, keyword)) {
            strcpy(records[count++], line);
        }
    }
    fclose(fp);

    if (count == 0) { printf("No matches found.\n"); return 0; }

    printf("\nFound %d record(s):\n", count);
    for (int i=0;i<count;i++) {
        char id[64], name[256], email[256], phone[64], date[64], status[64];
        parse_line(records[i], id, name, email, phone, date, status);
        printf("[%d] ID:%s Name:%s Email:%s Phone:%s RegDate:%s Status:%s\n", i+1, id, name, email, phone, date, status);
    }
    int idx;
    printf("Select index to delete (0 = cancel): ");
    if (scanf("%d", &idx) != 1) { while(getchar()!='\n'); return 0; }
    while(getchar()!='\n');
    if (idx <= 0 || idx > count) { printf("Cancelled.\n"); return 0; }

    char id[64], name[256], email[256], phone[64], date[64], status[64];
    parse_line(records[idx-1], id, name, email, phone, date, status);
    char c;
    printf("Confirm set ID %s (%s) to Inactive? (y/n): ", id, name);
    if (scanf(" %c", &c) != 1) { while(getchar()!='\n'); return 0; }
    while(getchar()!='\n');
    if (!(c=='y' || c=='Y')) { printf("Cancelled.\n"); return 0; }

    FILE *temp = fopen("temp.csv", "w");
    if (!temp) { printf("Cannot create temp file.\n"); return 0; }
    fprintf(temp, "ID,ParticipantName,Email,PhoneNumber,RegistrationDate,Status\n");
    for (int i=0;i<count;i++) {
        if (i == idx-1) {
            parse_line(records[i], id, name, email, phone, date, status);
            fprintf(temp, "%s,%s,%s,%s,%s,%s\n", id, name, email, phone, date, "Inactive");
        } else {
            fprintf(temp, "%s", records[i]);
        }
    }
    
    FILE *orig = fopen(CSV_FILE, "r");
    if (!orig) { fclose(temp); printf("Cannot reopen original file.\n"); return 0; }
    fgets(line, sizeof(line), orig);
    while (fgets(line, sizeof(line), orig)) {
        int skip = 0;
        for (int i=0;i<count;i++) if (strcmp(line, records[i]) == 0) { skip = 1; break; }
        if (!skip) fprintf(temp, "%s", line);
    }
    fclose(orig); fclose(temp);
    remove(CSV_FILE); rename("temp.csv", CSV_FILE);
    printf("Record set to Inactive.\n");
    return 1;
}

//update (edit) ของผู้เข้าร่วม (มีให้ยืนยันตัวตน)
int participant_edit_self() {
    char email[256], phone[64];
    printf("To edit your data, enter your registered email: ");
    if (fgets(email, sizeof(email), stdin) == NULL) return 0;
    trim_newline(email);
    printf("Enter your registered phone: ");
    if (fgets(phone, sizeof(phone), stdin) == NULL) return 0;
    trim_newline(phone);

    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) { printf("Cannot open file.\n"); return 0; }
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); // header
    char records[200][MAXLINE];
    int count = 0;
    int foundIndex = -1;
    while (fgets(line, sizeof(line), fp)) {
        strcpy(records[count], line);
        char id[64], name[256], e[256], p[64], date[64], status[64];
        if (parse_line(line, id, name, e, p, date, status)) {
            if (strcmp(e, email) == 0 && strcmp(p, phone) == 0) {
                foundIndex = count;
            }
        }
        count++;
    }
    fclose(fp);
    if (foundIndex == -1) { printf("No matching record found for given email & phone.\n"); return 0; }

    // write temp, updating the matched record via same interactive update (but verification already done)
    FILE *temp = fopen("temp.csv", "w");
    if (!temp) { printf("Cannot create temp file.\n"); return 0; }
    fprintf(temp, "ID,ParticipantName,Email,PhoneNumber,RegistrationDate,Status\n");
    for (int i=0;i<count;i++) {
        if (i == foundIndex) {
            char id[64], name[256], e[256], p[64], date[64], status[64];
            parse_line(records[i], id, name, e, p, date, status);
            update_info_interactive(temp, id, status);
        } else {
            fprintf(temp, "%s", records[i]);
        }
    }
    fclose(temp);
    remove(CSV_FILE); rename("temp.csv", CSV_FILE);
    printf("Your information has been updated.\n");
    return 1;
}

//delete (inactive) ของผู้เข้าร่วม (มีให้ยืนยันตัวตน)
int participant_delete_self() {
    char email[256], phone[64];
    printf("To delete your data, enter your registered email: ");
    if (fgets(email, sizeof(email), stdin) == NULL) return 0;
    trim_newline(email);
    printf("Enter your registered phone: ");
    if (fgets(phone, sizeof(phone), stdin) == NULL) return 0;
    trim_newline(phone);

    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) { printf("Cannot open file.\n"); return 0; }
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); // header
    char records[200][MAXLINE];
    int count = 0;
    int foundIndex = -1;
    while (fgets(line, sizeof(line), fp)) {
        strcpy(records[count], line);
        char id[64], name[256], e[256], p[64], date[64], status[64];
        if (parse_line(line, id, name, e, p, date, status)) {
            if (strcmp(e, email) == 0 && strcmp(p, phone) == 0) {
                foundIndex = count;
            }
        }
        count++;
    }
    fclose(fp);
    if (foundIndex == -1) { printf("No matching record found for given email & phone.\n"); return 0; }

    // confirm and set inactive
    char id[64], name[256], e[256], p[64], date[64], status[64];
    parse_line(records[foundIndex], id, name, e, p, date, status);
    char c;
    printf("Confirm delete (set Inactive) for ID %s, Name %s ? (y/n): ", id, name);
    if (scanf(" %c", &c) != 1) { while(getchar()!='\n'); return 0; }
    while(getchar()!='\n');
    if (!(c=='y' || c=='Y')) { printf("Cancelled.\n"); return 0; }

    FILE *temp = fopen("temp.csv", "w");
    if (!temp) { printf("Cannot create temp file.\n"); return 0; }
    fprintf(temp, "ID,ParticipantName,Email,PhoneNumber,RegistrationDate,Status\n");
    for (int i=0;i<count;i++) {
        if (i == foundIndex) {
            parse_line(records[i], id, name, e, p, date, status);
            fprintf(temp, "%s,%s,%s,%s,%s,%s\n", id, name, e, p, date, "Inactive");
        } else {
            fprintf(temp, "%s", records[i]);
        }
    }
    fclose(temp);
    remove(CSV_FILE); rename("temp.csv", CSV_FILE);
    printf("Your record has been set to Inactive.\n");
    return 1;
}

//แสดงข้อมูของแอดมิน แสดงทั้งหมด 
void display_all_admin() {
    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) { printf("Cannot open %s\n", CSV_FILE); return; }
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); // skip header
    char records[500][MAXLINE];
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strlen(line) > 1)
            strcpy(records[count++], line);
    }
    fclose(fp);

    if (count == 0) {
        printf("No participants found.\n");
        return;
    }

    // เรียงตาม ID
    qsort(records, count, sizeof(records[0]), compareByID);

    printf("\n===============================================================================================\n");
    printf("| %-6s | %-18s | %-25s | %-12s | %-12s | %-8s |\n", 
           "ID", "Name", "Email", "Phone", "Reg. Date", "Status");
    printf("===============================================================================================\n");
    for (int i = 0; i < count; i++) {
        char id[64], name[256], email[256], phone[64], date[64], status[64];
        if (!parse_line(records[i], id, name, email, phone, date, status)) continue;
        printf("| %-6s | %-18s | %-25s | %-12s | %-12s | %-8s |\n", 
               id, name, email, phone, date, status);
    }
    printf("===============================================================================================\n");
}
//แสดงข้อมูล ของผู้เข้าร่วม ไม่แสดงอีเมล เบอร์
void display_limited() {
    FILE *fp = fopen(CSV_FILE, "r");
    if (!fp) { printf("Cannot open %s\n", CSV_FILE); return; }
    char line[MAXLINE];
    fgets(line, sizeof(line), fp); // skip header
    char records[500][MAXLINE];
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strlen(line) > 1)
            strcpy(records[count++], line);
    }
    fclose(fp);

    if (count == 0) {
        printf("No participants found.\n");
        return;
    }

    // เรียงตาม ID
    qsort(records, count, sizeof(records[0]), compareByID);

    printf("\n===================================================================\n");
    printf("| %-6s | %-28s | %-12s | %-8s |\n", "ID", "Name", "Reg. Date", "Status");
    printf("===================================================================\n");
    for (int i = 0; i < count; i++) {
        char id[64], name[256], email[256], phone[64], date[64], status[64];
        if (!parse_line(records[i], id, name, email, phone, date, status)) continue;
        printf("| %-6s | %-28s | %-12s | %-8s |\n", id, name, date, status);
    }
    printf("===================================================================\n");
}

//เมนูของแอดมิน
int admin_menu() {
    int choice = 0;
    do {
        printf("\n===========================================================\n");
    printf("                        ADMIN MENU                            \n");
    printf("===========================================================\n");
    printf("| %-2s | %-50s |\n", "1", "Add participant");
    printf("| %-2s | %-50s |\n", "2", "Search participant (full info)");
    printf("| %-2s | %-50s |\n", "3", "Update participant (admin)");
    printf("| %-2s | %-50s |\n", "4", "Delete participant (admin - set Inactive)");
    printf("| %-2s | %-50s |\n", "5", "Display all participants (full)");
    printf("| %-2s | %-50s |\n", "6", "Run Unit Tests");
    printf("| %-2s | %-50s |\n", "7", "Run E2E Tests");
    printf("| %-2s | %-50s |\n", "8", "Logout to main");
    printf("===========================================================\n");
    printf("Choose option: ");

        if (scanf("%d", &choice) != 1) { while(getchar()!='\n'); choice = -1; }
        while(getchar()!='\n');
        switch (choice) {
            case 1: add_participant_interactive(); break;
            case 2: search_controller(1); break;
            case 3: update_direct_admin(); break;
            case 4: delete_direct_admin(); break;
            case 5: display_all_admin(); break;
            case 6: run_full_unit_test_suite(); break;
            case 7: run_full_e2e_test_suite(); break;
            case 8: printf("Logging out admin...\n"); break;
            default:
                printf("Invalid choice. Please select 1-8 only.\n");
                break;
        }
    } while (choice != 8);
    return 0;
}
//เมนูของผู้เข้าร่วม
int participant_menu() {
    int choice = 0;
    do {
        printf("\n===========================================================\n");
    printf("                     PARTICIPANT MENU                         \n");
    printf("===========================================================\n");
    printf("| %-2s | %-50s |\n", "1", "Register (Add participant)");
    printf("| %-2s | %-50s |\n", "2", "Show participants (ID, Name, RegDate, Status)");
    printf("| %-2s | %-50s |\n", "3", "Search participant (limited view)");
    printf("| %-2s | %-50s |\n", "4", "Edit your info (verify email+phone)");
    printf("| %-2s | %-50s |\n", "5", "Delete your info (verify email+phone)");
    printf("| %-2s | %-50s |\n", "6", "Run Unit Tests");
    printf("| %-2s | %-50s |\n", "7", "Run E2E Tests");
    printf("| %-2s | %-50s |\n", "8", "Exit to main");
    printf("===========================================================\n");
    printf("Choose option: ");

        if (scanf("%d", &choice) != 1) { while(getchar()!='\n'); choice = -1; }
        while(getchar()!='\n');
        switch (choice) {
            case 1: add_participant_interactive(); break;
            case 2: display_limited(); break;
            case 3: search_controller(0); break;
            case 4: participant_edit_self(); break;
            case 5: participant_delete_self(); break;
            case 6: run_full_unit_test_suite(); break;
            case 7: run_full_e2e_test_suite(); break;
            case 8: printf("Returning to main menu...\n"); break;
            default:
                printf("Invalid choice. Please select 1-8 only.\n");
                break;
        }
    } while (choice != 8);
    return 0;
}

/* ================================================================
   UNIT TEST : ADD + UPDATE + VALIDATION + SELF-EDIT
   ================================================================ */
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

void run_full_unit_test_suite() {
    printf("\n=== RUNNING FULL UNIT TEST SUITE ===\n");
    test_case_count = 0;
    test_fail_count = 0;

    backup_csv_full();
    setup_test_csv_full();

    /* ------------------ ADD TESTS ------------------ */
    printf("\n--- ADD TESTS ---\n");
    print_test_result("Add: Valid data", saveCSV("P0004", "David", "david@test.com", "0844444444", "2025-10-05", "Active") == 1);
    print_test_result("Add: Invalid email format", validateEmail("bademail@") == 0);
    print_test_result("Add: Invalid phone format", validatePhone("12345") == 0);
    print_test_result("Add: Duplicate active email detected", isEmailDuplicate("alice@test.com") == 1);
    print_test_result("Add: Duplicate inactive email allowed", isEmailDuplicate("bob@test.com") == 0);
    print_test_result("Add: Duplicate active phone detected", isPhoneDuplicate("0812345678") == 1);
    print_test_result("Add: Duplicate inactive phone allowed", isPhoneDuplicate("0822222222") == 0);

    /* ------------------ UPDATE TESTS ------------------ */
    printf("\n--- UPDATE TESTS ---\n");
    print_test_result("Update: Valid name change", simulate_update_record("alice@test.com", NULL, NULL, "AliceUpdated") == 1);
    print_test_result("Update: Invalid email rejected", validateEmail("wrong@@com") == 0);
    print_test_result("Update: Invalid phone rejected", validatePhone("abcd12345") == 0);
    print_test_result("Update: Duplicate active email detected", isEmailDuplicateExcept("charlie@test.com", "P0001") == 1);
    print_test_result("Update: Duplicate inactive email allowed", isEmailDuplicateExcept("bob@test.com", "P0001") == 0);
    print_test_result("Update: Duplicate active phone detected", isPhoneDuplicateExcept("0833333333", "P0001") == 1);
    print_test_result("Update: Duplicate inactive phone allowed", isPhoneDuplicateExcept("0822222222", "P0001") == 0);

    /* ------------------ VALIDATION TESTS ------------------ */
    printf("\n--- EMAIL & PHONE VALIDATION TESTS ---\n");
    // email
    print_test_result("Email: missing @", validateEmail("email.com") == 0);
    print_test_result("Email: starts with @", validateEmail("@mail.com") == 0);
    print_test_result("Email: ends with @", validateEmail("email@") == 0);
    print_test_result("Email: missing domain part", validateEmail("email@com") == 0);
    print_test_result("Email: space in address", validateEmail("e mail@test.com") == 0);
    print_test_result("Email: multiple @ signs", validateEmail("email@@test.com") == 0);
    print_test_result("Email: missing dot after @", validateEmail("email@testcom") == 0);
    print_test_result("Email: only domain no name", validateEmail("@example.com") == 0);
    print_test_result("Email: only name no domain", validateEmail("example@") == 0);
    print_test_result("Email: dot at end", validateEmail("user@test.com.") == 1); 
    print_test_result("Email: subdomain valid", validateEmail("user@sub.test.com") == 1);
    print_test_result("Email: valid normal", validateEmail("valid.email@test.com") == 1);
    print_test_result("Email: uppercase valid", validateEmail("USER@TEST.COM") == 1);
    
    // phone
    print_test_result("Phone: letters invalid", validatePhone("0812abc567") == 0);
    print_test_result("Phone: too short", validatePhone("08123") == 0);
    print_test_result("Phone: invalid prefix", validatePhone("0512345678") == 0);
    print_test_result("Phone: valid 08", validatePhone("0812345678") == 1);
    print_test_result("Phone: valid 09", validatePhone("0912345678") == 1);
    print_test_result("Phone: valid 06", validatePhone("0612345678") == 1);

    /* ------------------ PARTICIPANT SELF-EDIT ------------------ */
    printf("\n--- PARTICIPANT SELF-EDIT TESTS ---\n");
    print_test_result("Self-edit: valid name change", participant_edit_self_sim("alice@test.com", "0812345678", "AliceSelfEdit", NULL, NULL) == 1);
    print_test_result("Self-edit: email/phone mismatch", participant_edit_self_sim("wrong@test.com", "0812345678", "X", NULL, NULL) == 0);
    print_test_result("Self-edit: invalid new email", participant_edit_self_sim("alice@test.com", "0812345678", NULL, "bademail@", NULL) == -1);
    print_test_result("Self-edit: duplicate active email", participant_edit_self_sim("alice@test.com", "0812345678", NULL, "charlie@test.com", NULL) == -2);
    print_test_result("Self-edit: duplicate inactive email allowed", participant_edit_self_sim("alice@test.com", "0812345678", NULL, "bob@test.com", NULL) == 1);
    print_test_result("Self-edit: invalid phone", participant_edit_self_sim("charlie@test.com", "0833333333", NULL, NULL, "12abcd5678") == -3);
    print_test_result("Self-edit: duplicate active phone", participant_edit_self_sim("charlie@test.com", "0833333333", NULL, NULL, "0812345678") == -4);
    print_test_result("Self-edit: duplicate inactive phone allowed", participant_edit_self_sim("charlie@test.com", "0833333333", NULL, NULL, "0822222222") == 1);

    /* ------------------ RESULT SUMMARY ------------------ */
    printf("\n====================================================\n");
    printf("Total Tests: %d, Failed: %d\n", test_case_count, test_fail_count);
    if (test_fail_count == 0)
        printf("ALL TESTS PASSED SUCCESSFULLY!\n");
    else
        printf("SOME TESTS FAILED. CHECK LOG ABOVE.\n");
    printf("====================================================\n");

    restore_csv_full();
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

/* ================================================================
   E2E TEST SUITE: Simulates full Admin and Participant workflows
   ================================================================ */
void run_full_e2e_test_suite() {
    printf("\n=== RUNNING FULL E2E TEST SUITE (ADMIN & PARTICIPANT) ===\n");
    test_case_count = 0;
    test_fail_count = 0;

    backup_csv_full();
    setup_test_csv_full(); // Start with P0001, P0002(Inactive), P0003

    char newID[64];
    
    /* ------------------ PARTICIPANT WORKFLOW E2E ------------------ */
    printf("\n--- PARTICIPANT E2E: ADD, SELF-EDIT, SELF-DELETE ---\n");
    
    // E2E Step 1: Register (Add)
    int add_success = simulate_add_record("Test User E2E", "e2e.user@test.com", "0665554444", newID, sizeof(newID));
    print_test_result("E2E 1.1: Register new participant", add_success == 1);
    print_test_result("E2E 1.2: Verify new record is Active", check_record_status(newID) == 1);

    // E2E Step 2: Self-Edit (Edit Name & Phone)
    int edit_success = participant_edit_self_sim("e2e.user@test.com", "0665554444", "User Edited", NULL, "0661112222");
    print_test_result("E2E 2.1: Self-edit successful", edit_success == 1);
    
    // E2E Step 3: Self-Delete (Set Inactive)
    // เนื่องจากไม่มีฟังก์ชันจำลอง self-delete เราจะใช้ simulate_delete_record แทนเพื่อตรวจสอบสถานะสุดท้าย
    // ในการใช้งานจริง ผู้เข้าร่วมจะเข้าถึงฟังก์ชัน participant_delete_self()
    int delete_sim_success = simulate_delete_record(newID);
    print_test_result("E2E 3.1: Self-delete successful (simulated)", delete_sim_success == 1);
    print_test_result("E2E 3.2: Verify final status is Inactive", check_record_status(newID) == 2);


    /* ------------------ ADMIN WORKFLOW E2E ------------------ */
    printf("\n--- ADMIN E2E: UPDATE & DELETE ON EXISTING RECORD ---\n");
    const char *adminTargetID = "P0003"; // Charlie, charlie@test.com, 0833333333

    // E2E Step 4: Admin Update (Change Email/Phone for P0003)
    // จำลองการอัปเดต Email/Phone ที่ถูกต้อง
    int admin_update = simulate_update_record("charlie@test.com", "charlie.upd@test.com", "0839999999", NULL);
    print_test_result("E2E 4.1: Admin update successful", admin_update == 1);
    
    // E2E Step 5: Admin Delete (Set P0003 to Inactive)
    int admin_delete = simulate_delete_record(adminTargetID);
    print_test_result("E2E 5.1: Admin delete successful", admin_delete == 1);
    print_test_result("E2E 5.2: Verify P0003 status is Inactive", check_record_status(adminTargetID) == 2);


    /* ------------------ RESULT SUMMARY ------------------ */
    printf("\n====================================================\n");
    printf("E2E Test Summary: Total Tests: %d, Failed: %d\n", test_case_count, test_fail_count);
    if (test_fail_count == 0)
        printf("ALL E2E TESTS PASSED SUCCESSFULLY!\n");
    else
        printf("SOME E2E TESTS FAILED. CHECK LOG ABOVE.\n");
    printf("====================================================\n");

    restore_csv_full();
}

int main()
{
    createfile();
    readCSV();
    printf("\n============================================\n");
    printf("       WELCOME TO SEMINAR SYSTEM\n");
    printf("============================================\n");
    printf("Are you an admin? (y/n): ");
    char ch;
    if (scanf(" %c", &ch) != 1) return 0;
    while(getchar()!='\n');

    if (ch == 'y' || ch == 'Y') {
        if (admin_login()) {
            // stay in admin menu until logout
            admin_menu();
            // after logout, return to main menu selection
        } else {
            printf("Admin login failed. Entering participant mode.\n");
            participant_menu();
        }
    } else {
        participant_menu();
    }

    // After finishing one session, offer to choose again or exit
    int again = 0;
    do {
        printf("\nDo you want to continue? (1 = yes, 0 = exit): ");
        if (scanf("%d", &again) != 1) break;
        while(getchar()!='\n');
        if (again == 1) {
            printf("Are you an admin? (y/n): ");
            if (scanf(" %c", &ch) != 1) break;
            while(getchar()!='\n');
            if (ch == 'y' || ch == 'Y') {
                if (admin_login()) admin_menu();
                else { printf("Login failed. Participant menu.\n"); participant_menu(); }
            } else participant_menu();
        }
    } while (again == 1);

    printf("Goodbye.\n");
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}


