#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<assert.h>
#include<time.h>

int createfile();
int readCSV();
int saveCSV(const char *id, const char *name, const char *email, const char *phone, const char *regDate, const char *status);
int isValidEmail(const char *email);
int validateEmail(char *email);
int validatePhone(char *phone);
int isLeapYear(int year);
int validateDate(int y, int m, int d);
char *getToday(char *buf, size_t bufsz);
int generateID(char *out_id, size_t out_sz);
int isEmailDuplicate(const char *email);
int admin_login();
void trim_newline(char *s);
int parse_line(const char *line, char *id, char *name, char *email, char *phone, char *date, char *status);


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
int isValidEmail(const char *email) {
    const char *at = strchr(email, '@');
    if (!at) return 0;                // ต้องมี '@'
    if (at == email) return 0;        // ห้ามขึ้นต้นด้วย '@'
    if (*(at+1) == '\0') return 0;    // ห้ามจบด้วย '@'
    if (strchr(at, '.') == NULL) return 0; // ต้องมี '.' หลัง '@'
    return 1;
}

int validateEmail (char *email)
{
    return isValidEmail(email);
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

// parse a CSV line into 6 fields (ID,Name,Email,Phone,Date,Status)
// returns 1 if parsed, 0 otherwise
int parse_line(const char *line, char *id, char *name, char *email, char *phone, char *date, char *status) {
    if (!line) return 0;
    char tmp[512];
    strncpy(tmp, line, sizeof(tmp)-1);
    tmp[sizeof(tmp)-1]=0;
    trim_newline(tmp);

    char *token;
    token = strtok(tmp, ",");
    if (!token) return 0;
    strcpy(id, token);

    token = strtok(NULL, ",");
    if (!token) return 0;
    strcpy(name, token);

    token = strtok(NULL, ",");
    if (!token) return 0;
    strcpy(email, token);

    token = strtok(NULL, ",");
    if (!token) return 0;
    strcpy(phone, token);

    token = strtok(NULL, ",");
    if (!token) return 0;
    strcpy(date, token);

    token = strtok(NULL, ",");
    if (!token) {
        // If no status present, assume Active
        strcpy(status, "Active");
    } else {
        strcpy(status, token);
    }
    return 1;
}

// สร้าง ID ถัดไป จากไฟล์ (รูปแบบ P0001)
int generateID(char *out_id, size_t out_sz) {
    FILE *fp = fopen("Seminar.csv", "r");
    if (!fp) {
        // if cannot open, start from P0001
        snprintf(out_id, out_sz, "P%04d", 1);
        return 1;
    }
    char line[512];
    int maxnum = 0;
    fgets(line, sizeof(line), fp); // skip header
    while (fgets(line, sizeof(line), fp)) {
        char id[50], name[200], email[200], phone[50], date[50], status[20];
        if (parse_line(line, id, name, email, phone, date, status)) {
            // id like P0001
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

// ตรวจสอบอีเมลซ้ำ ไม่ว่า Active/Inactive (ตามที่ผู้ใช้ขอ: แจ้งว่าเคยลงทะเบียนแล้ว)
int isEmailDuplicate(const char *email) {
    FILE *fp = fopen("Seminar.csv", "r");
    if (!fp) return 0;
    char line[512];
    fgets(line, sizeof(line), fp); // header
    while (fgets(line, sizeof(line), fp)) {
        char id[50], name[200], e[200], phone[50], date[50], status[20];
        if (parse_line(line, id, name, e, phone, date, status)) {
            if (strcmp(e, email) == 0) {
                fclose(fp);
                return 1; // พบอีเมลนี้ในระบบ
            }
        }
    }
    fclose(fp);
    return 0;
}

// ฟังก์ชันตรวจสิทธิ์ admin แบบง่าย
int admin_login() {
    char user[50], pass[50];
    printf("\nAdmin login required.\n");
    printf("Username: ");
    if (scanf("%49s", user) != 1) { while(getchar()!='\n'); return 0; }
    printf("Password: ");
    if (scanf("%49s", pass) != 1) { while(getchar()!='\n'); return 0; }
    while(getchar()!='\n');

    // ค่า default — คุณสามารถเปลี่ยนได้ตามต้องการ
    if (strcmp(user, "admin") == 0 && strcmp(pass, "1234") == 0) {
        printf("\nLogin successful.\n");
        return 1;
    } else {
        printf("Invalid credentials.\n");
        return 0;
    }
}

//เพิ่มข้อมูลผู้เข้าร่วมใหม่
int add_info()
{
    FILE *fp;
    char name[50];
    char email[50];
    char phone[20];
    char regDate[20];
    char id[20];

    printf("Please enter your name: ");
    scanf(" %[^\n]", name);

    do
    {
        printf("Please enter your email: ");
        scanf(" %[^\n]", email);
        if (!validateEmail(email)) printf("Invalid email! Must contain @ and . after @\n");
    }
    while (!validateEmail(email));

    do 
    {
        printf("Please enter your phone number: ");
        scanf(" %[^\n]", phone);
        if (!validatePhone(phone)) printf("Phone must start with 09/08/06 and be 10 digits!\n");
    } 
    while (!validatePhone(phone)); 

    // สร้าง ID ใหม่
    generateID(id, sizeof(id));
    // วันที่ปัจจุบัน
    getToday(regDate, sizeof(regDate));

    // บันทึก: status = Active
    if (!saveCSV(id, name, email, phone, regDate, "Active")) {
        printf("Failed to save data.\n");
        return 0;
    }

    printf("Registration successful!\n");
    printf("Your ID: %s\n", id);
    printf("Registration Date: %s\n", regDate);
    return 1 ;
}

//อัพเดทข้อมูลผู้เข้าร่วม
// ฟังก์ชันอัปเดต: แก้บางช่องได้, ตรวจอีเมลซ้ำ, ยืนยันก่อนบันทึก
int update_info(FILE *temp, const char *orig_id, const char *orig_status)
{
    char newName[100], newEmail[100], newPhone[20], newDate[20];
    char currentName[100] = "", currentEmail[100] = "", currentPhone[20] = "", currentDate[20] = "";
    int confirm_ok = 0;

    // โหลดข้อมูลปัจจุบันจากไฟล์
    FILE *fp = fopen("Seminar.csv", "r");
    if (!fp) { printf("Cannot open file!\n"); return 0; }

    char line[512];
    fgets(line, sizeof(line), fp); // skip header
    while (fgets(line, sizeof(line), fp)) {
        char id[50], name[200], email[200], phone[50], date[50], status[20];
        if (parse_line(line, id, name, email, phone, date, status)) {
            if (strcmp(id, orig_id) == 0) {
                strcpy(currentName, name);
                strcpy(currentEmail, email);
                strcpy(currentPhone, phone);
                strcpy(currentDate, date);
                break;
            }
        }
    }
    fclose(fp);

    // วนลูปจนกว่าจะยืนยันว่าข้อมูลถูกต้อง
    do {
        printf("\n=== Update Record (ID: %s) ===\n", orig_id);
        printf("(Press Enter to keep current value)\n\n");

        // ---------- แก้ไขชื่อ ----------
        printf("Current Name: %s\nNew Name: ", currentName);
        fgets(newName, sizeof(newName), stdin);
        newName[strcspn(newName, "\n")] = 0;
        if (strlen(newName) == 0) strcpy(newName, currentName);

        // ---------- แก้ไขอีเมล ----------
        do {
            printf("\nCurrent Email: %s\nNew Email: ", currentEmail);
            fgets(newEmail, sizeof(newEmail), stdin);
            newEmail[strcspn(newEmail, "\n")] = 0;
            if (strlen(newEmail) == 0) { strcpy(newEmail, currentEmail); break; }

            if (!validateEmail(newEmail)) {
                printf("Invalid email! Must contain @ and a dot after @\n");
                continue;
            }

            // ตรวจอีเมลซ้ำ (ยกเว้นของตัวเอง)
            FILE *fcheck = fopen("Seminar.csv", "r");
            int duplicate = 0;
            if (fcheck) {
                char l[512];
                fgets(l, sizeof(l), fcheck); // skip header
                while (fgets(l, sizeof(l), fcheck)) {
                    char id[50], name[200], e[200], phone[50], date[50], status[20];
                    if (parse_line(l, id, name, e, phone, date, status)) {
                        if (strcmp(e, newEmail) == 0 && strcmp(id, orig_id) != 0) {
                            duplicate = 1;
                            break;
                        }
                    }
                }
                fclose(fcheck);
            }

            if (duplicate) {
                printf(" This email already belongs to another participant!\n");
                continue;
            }

            break;
        } while (1);

        // ---------- แก้ไขเบอร์โทร ----------
        do {
            printf("\nCurrent Phone: %s\nNew Phone: ", currentPhone);
            fgets(newPhone, sizeof(newPhone), stdin);
            newPhone[strcspn(newPhone, "\n")] = 0;
            if (strlen(newPhone) == 0) { strcpy(newPhone, currentPhone); break; }
            if (!validatePhone(newPhone)) {
                printf("Phone must start with 09/08/06 and be 10 digits!\n");
                continue;
            }
            break;
        } while (1);

        // ---------- แก้ไขวันที่ ----------
        printf("\nCurrent Registration Date: %s\nNew Date (YYYY-MM-DD or 0 for today): ", currentDate);
        fgets(newDate, sizeof(newDate), stdin);
        newDate[strcspn(newDate, "\n")] = 0;
        if (strlen(newDate) == 0) {
            strcpy(newDate, currentDate);
        } else if (strcmp(newDate, "0") == 0) {
            getToday(newDate, sizeof(newDate));
        } else {
            int y, m, d;
            if (sscanf(newDate, "%d-%d-%d", &y, &m, &d) != 3 || !validateDate(y, m, d)) {
                printf("Invalid date format! Using old date.\n");
                strcpy(newDate, currentDate);
            }
        }

        // ---------- แสดงข้อมูลที่กรอกทั้งหมด ----------
        printf("\nPlease confirm the updated information:\n");
        printf("------------------------------------------------------------\n");
        printf("ID: %s\n", orig_id);
        printf("Name: %s\n", newName);
        printf("Email: %s\n", newEmail);
        printf("Phone: %s\n", newPhone);
        printf("Registration Date: %s\n", newDate);
        printf("Status: %s\n", orig_status);
        printf("------------------------------------------------------------\n");

        char confirm;
        printf("Is this information correct? (y/n): ");
        scanf(" %c", &confirm);
        while (getchar() != '\n'); // clear buffer

        if (confirm == 'y' || confirm == 'Y') {
            confirm_ok = 1;
        } else {
            printf("\nLet's re-enter the information again.\n");
        }

    } while (!confirm_ok);

    // ---------- บันทึกข้อมูลใหม่ ----------
    fprintf(temp, "%s,%s,%s,%s,%s,%s\n", orig_id, newName, newEmail, newPhone, newDate, orig_status);
    printf(" Record updated successfully (ID: %s)\n", orig_id);
    return 1;
}

// เปลี่ยนสถานะเป็น Inactive (ยืนยันก่อน)
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
        // ยกเลิก -> เขียนบรรทัดเดิม
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
int search_info()
{
    FILE *fp = fopen("Seminar.csv","r");
    if (!fp) { printf("Cannot open file!\n"); return 0; }

    char keyword [100];
    char line [512];
    char records[200][512]; // เก็บ record ที่ตรง keyword
    int count = 0;

    printf("Enter Name/Email/Phone/Date/ID to search: ");
    if (scanf(" %[^\n]", keyword) != 1) { while(getchar()!='\n'); fclose(fp); return 0; }

    fgets(line,sizeof(line),fp); // อ่าน header
    char header[512];
    strcpy(header,line);

    // เก็บ record ที่ตรง keyword
    while (fgets(line,sizeof(line),fp)) {
        if (strstr(line,keyword)) {
            strcpy(records[count++],line);
        }
    }
    fclose(fp);

    if (count == 0) {
        printf("No record found!\n");
        return 0;
    }

    // แสดง record ทั้งหมดพร้อม index
    printf("\n--- Found %d record(s) ---\n", count);
    for (int i=0;i<count;i++) {
        char tmp[512];
        strcpy(tmp,records[i]);
        char id[50], name[200], email[200], phone[50], date[50], status[20];
        parse_line(tmp, id, name, email, phone, date, status);

        printf("\n[%d] Participant Information\n", i+1);
        printf("ID: %s\n", id);
        printf("Name: %s\n", name);
        printf("Email: %s\n", email);
        printf("Phone Number: %s\n", phone);
        printf("Registration Date: %s\n", date);
        printf("Status: %s\n", status);
    }

    int index, choice;
    printf("\nSelect record index to act on (0 = skip): ");
    if (scanf("%d",&index) != 1) { while(getchar()!='\n'); return 0; }
    while(getchar()!='\n');
    if (index <= 0 || index > count) {
        printf("Skip action.\n");
        return 0;
    }

    // ต้องสร้าง temp file สำหรับเขียนไฟล์ใหม่
    FILE *temp = fopen("temp.csv","w");
    if (!temp) { printf("Cannot create temp file!\n"); return 0; }
    fprintf(temp,"%s",header);

    for (int i=0;i<count;i++) {
        if (i == index-1) { // record ที่เลือก
            char chosen[512]; strcpy(chosen, records[i]);
            char id[50], name[200], email[200], phone[50], date[50], status[20];
            parse_line(chosen, id, name, email, phone, date, status);

            printf("\nWhat do you want to do?\n1.Update\n2.Delete (set Inactive)\n3.Skip\nChoice: ");
            if (scanf("%d",&choice) != 1) { while(getchar()!='\n'); fprintf(temp,"%s",records[i]); continue; }
            while(getchar()!='\n');

            if (choice==1) {
                if (!admin_login()) { printf("Admin authentication failed. Update aborted.\n"); fprintf(temp,"%s",records[i]); continue; }
                update_info(temp, id, status);
            } else if (choice==2) {
                if (!admin_login()) { printf("Admin authentication failed. Delete aborted.\n"); fprintf(temp,"%s",records[i]); continue; }
                set_inactive(temp, records[i]);
            } else {
                fprintf(temp,"%s",records[i]);
            }
        } else {
            fprintf(temp,"%s",records[i]);
        }
    }

    // นำ record อื่น ๆ จากไฟล์เก่ามาเขียน (exclude those matched)
    fp = fopen("Seminar.csv","r");
    if (!fp) { fclose(temp); printf("Cannot reopen original file!\n"); return 0; }
    fgets(line,sizeof(line),fp); // skip header
    while(fgets(line,sizeof(line),fp)) {
        int skip=0;
        for (int i=0;i<count;i++) {
            if (strcmp(line,records[i])==0) { skip=1; break; }
        }
        if (!skip) fprintf(temp,"%s",line);
    }
    fclose(fp);
    fclose(temp);

    remove("Seminar.csv");
    rename("temp.csv","Seminar.csv");

    return 1;
}

//update หน้าเมนู
int update_direct() 
{
    if (!admin_login()) {
        printf("Admin authentication required to update.\n");
        return 0;
    }

    char keyword[100];
    char line[512];
    char records[200][512];
    int count = 0;

    FILE *fp = fopen("Seminar.csv","r");
    if (!fp) { printf("Cannot open file!\n"); return 0; }

    printf("Enter keyword to update (Name/Email/Phone/Date/ID): ");
    if (scanf(" %[^\n]", keyword) != 1) { while(getchar()!='\n'); fclose(fp); return 0; }

    fgets(line,sizeof(line),fp); // header
    char header[512]; strcpy(header,line);

    while(fgets(line,sizeof(line),fp)) {
        if (strstr(line,keyword)) {
            strcpy(records[count++],line);
        }
    }
    fclose(fp);

    if (count==0) { printf("No record found!\n"); return 0; }

    printf("\n--- Found %d record(s) ---\n", count);
    for (int i=0;i<count;i++) {
        char tmp[512]; strcpy(tmp,records[i]);
        char id[50], name[200], email[200], phone[50], date[50], status[20];
        parse_line(tmp, id, name, email, phone, date, status);

        printf("\n[%d] Participant Information\n", i+1);
        printf("ID: %s\n", id);
        printf("Name: %s\n", name);
        printf("Email: %s\n", email);
        printf("Phone Number: %s\n", phone);
        printf("Registration Date: %s\n", date);
        printf("Status: %s\n", status);
    }

    int index;
    printf("\nSelect record index to update (0 = skip): ");
    if (scanf("%d",&index) != 1) { while(getchar()!='\n'); return 0; }
    while(getchar()!='\n');
    if (index<=0 || index>count) { printf("Skip update.\n"); return 0; }

    FILE *temp = fopen("temp.csv","w");
    if (!temp) { printf("Cannot create temp file!\n"); return 0; }
    fprintf(temp,"%s",header);

    for (int i=0;i<count;i++) {
        if (i==index-1) {
            char id[50], name[200], email[200], phone[50], date[50], status[20];
            parse_line(records[i], id, name, email, phone, date, status);
            update_info(temp, id, status);
        } else fprintf(temp,"%s",records[i]);
    }

    // นำ record อื่น ๆ จากไฟล์เก่าเขียน
    fp = fopen("Seminar.csv","r");
    fgets(line,sizeof(line),fp); // skip header
    while(fgets(line,sizeof(line),fp)) {
        int skip=0;
        for (int i=0;i<count;i++) {
            if (strcmp(line,records[i])==0) { skip=1; break; }
        }
        if (!skip) fprintf(temp,"%s",line);
    }
    fclose(fp);
    fclose(temp);

    remove("Seminar.csv");
    rename("temp.csv","Seminar.csv");

    return 1;
}

// delete direct (set Inactive) — requires admin
int delete_direct() 
{
    if (!admin_login()) {
        printf("Admin authentication required to delete.\n");
        return 0;
    }

    char keyword[100];
    char line[512];
    char records[200][512];
    int count = 0;

    FILE *fp = fopen("Seminar.csv","r");
    if (!fp) { printf("Cannot open file!\n"); return 0; }

    printf("Enter keyword to delete (Name/Email/Phone/Date/ID): ");
    if (scanf(" %[^\n]", keyword) != 1) { while(getchar()!='\n'); fclose(fp); return 0; }

    fgets(line,sizeof(line),fp); // header
    char header[512]; strcpy(header,line);

    while(fgets(line,sizeof(line),fp)) {
        if (strstr(line,keyword)) {
            strcpy(records[count++],line);
        }
    }
    fclose(fp);

    if (count==0) { printf("No record found!\n"); return 0; }

    printf("\n--- Found %d record(s) ---\n", count);
    for (int i=0;i<count;i++) {
        char tmp[512]; strcpy(tmp,records[i]);
        char id[50], name[200], email[200], phone[50], date[50], status[20];
        parse_line(tmp, id, name, email, phone, date, status);

        printf("\n[%d] Participant Information\n", i+1);
        printf("ID: %s\n", id);
        printf("Name: %s\n", name);
        printf("Email: %s\n", email);
        printf("Phone Number: %s\n", phone);
        printf("Registration Date: %s\n", date);
        printf("Status: %s\n", status);
    }

    int index;
    printf("\nSelect record index to delete (0 = skip): ");
    if (scanf("%d",&index) != 1) { while(getchar()!='\n'); return 0; }
    while(getchar()!='\n');
    if (index<=0 || index>count) { printf("Skip delete.\n"); return 0; }

    FILE *temp = fopen("temp.csv","w");
    if (!temp) { printf("Cannot create temp file!\n"); return 0; }
    fprintf(temp,"%s",header);

    for (int i=0;i<count;i++) {
        if (i==index-1) set_inactive(temp, records[i]); // เขียนเป็น Inactive หรือยกเลิกตาม confirm
        else fprintf(temp,"%s",records[i]);
    }

    // เขียน record อื่น ๆ จากไฟล์เก่า
    fp = fopen("Seminar.csv","r");
    fgets(line,sizeof(line),fp); // skip header
    while(fgets(line,sizeof(line),fp)) {
        int skip=0;
        for (int i=0;i<count;i++) {
            if (strcmp(line,records[i])==0) { skip=1; break; }
        }
        if (!skip) fprintf(temp,"%s",line);
    }
    fclose(fp);
    fclose(temp);

    remove("Seminar.csv");
    rename("temp.csv","Seminar.csv");

    return 1;
}

//แสดงข้อมูลทั้งหมด
int display_all()
{
    FILE *fp = fopen("Seminar.csv","r");
    if (fp == NULL)
    {
        printf("Cannot open the file\n");
        return 0;
    }

    char line[512];

    printf("\n======================================================================================================\n");
    printf("| %-6s | %-18s | %-25s | %-12s | %-12s | %-10s |\n",
           "ID", "Name", "Email", "Phone", "Reg. Date", "Status");
    printf("======================================================================================================\n");

    // ข้าม header
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp))
    {
        char id[50], name[200], email[200], phone[50], date[50], status[20];
        if (!parse_line(line, id, name, email, phone, date, status)) continue;

        printf("| %-6s | %-18s | %-25s | %-12s | %-12s | %-10s |\n",
               id, name, email, phone, date, status);
    }

    printf("======================================================================================================\n");

    fclose(fp);
    return 1;
}

//แสดงเมนู 
int display_menu()
{
    int choice ;
    do {
        printf ("\n===========================");
        printf("\n| Welcome to the seminar! |\n");
        printf ("===========================\n");
        printf("\n========= MENU ========== \n");
        printf("| 1 | Show all information\n");
        printf("| 2 | Add participants\n");
        printf("| 3 | Search for participants information\n");
        printf("| 4 | Update participant (admin only)\n");
        printf("| 5 | Delete participant (set Inactive) (admin only)\n");
        printf("| 6 | UNIT TEST \n");
        printf("| 7 | E2E TEST \n");
        printf("| 8 | Exit\n");
        printf("---> Please select an option: ");
        if (scanf("%d", &choice) != 1) { while(getchar()!='\n'); choice = -1; }
        while(getchar()!='\n');

        switch (choice) {
            case 1: display_all(); break;
            case 2: add_info(); break;
            case 3: search_info(); break;
            case 4: update_direct(); break;
            case 5: delete_direct(); break;
            //case 6: unit_test(); break;
            //case 7: e2e_test(); break;
            case 8: printf("\nExit program!\n"); break;
            default: if (choice!=-1) printf("\nInvalid choice!\n");
        }
    } while (choice != 8);
    return 0;
}

int main()
{
    createfile();
    readCSV();
    display_menu();
    return 0;
}


