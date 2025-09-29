#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
//ระบบจัดการข้อมูลผู้เข้าร่วมสัมมนา

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
        fprintf(fp, "ParticipantName,Email,PhoneNumber,RegistrationDate\n");
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
    else
    {
        printf("File: %p\n",fp);
        printf("File opened successfully\n");
    }
    fclose (fp);
    return 1;
}

//บันทึกข้อมูลผู้เข้าร่วมสัมมนาลงไฟล์ CSV
int saveCSV(char *name, char *email, char *phone, char *regDate)
{

    FILE *fp;
    fp = fopen("Seminar.csv","a");
    if (fp==NULL)
    {
        printf("Cannot open file\n");
        return 0;
    }
    fprintf(fp,"%s,%s,%s,%s\n", name, email, phone, regDate);

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
int validatePhone (char *phone)
{
    if (strlen(phone) != 10) return 0;
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

//เพิ่มข้อมูลผู้เข้าร่วมใหม่
int add_info()
{
    FILE *fp;
    char name[50];
    char email[50];
    char phone[20];
    int d, m, y ;
    char regDate[20];

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

    do 
    {
        printf("Please enter the registration date (YYYY-MM-DD): ");
    if (scanf("%d-%d-%d", &y, &m, &d) != 3) 
    {
        printf("Invalid format! Use YYYY-MM-DD\n");
        while (getchar() != '\n'); // ล้างบัฟเฟอร์
        continue;
    }
    if (!validateDate(y,m,d)) 
    {
        printf("Invalid date!\n");
    }
    } 
    while (!validateDate(y,m,d));
    sprintf(regDate,"%04d-%02d-%02d",y,m,d);

    saveCSV(name,email,phone,regDate);
    printf("Data added successfully!\n");

    return 1 ;
}

//อัพเดทข้อมูลผู้เข้าร่วม
int update_info(FILE *temp)
{
    char newName[50], newEmail[50], newPhone[20], newDate[20];
    int y, m, d ;

    printf("Enter new name : ");
    scanf("%[^\n]", newName);

    do {
        printf("Enter new email: ");
        scanf(" %[^\n]", newEmail);
        if (!validateEmail(newEmail)) printf("Invalid email! Must contain @ and . after @\n");
    } while (!validateEmail(newEmail));

    do {
        printf("Enter new phone: ");
        scanf(" %[^\n]", newPhone);
        if (!validatePhone(newPhone)) printf("Phone must start with 09/08/06 and be 10 digits!\n");
    } while (!validatePhone(newPhone));

    do {
        printf("Enter new date (YYYY-MM-DD): ");
        scanf("%d-%d-%d", &y, &m, &d);
        if (!validateDate(y,m,d)) printf("Invalid date!\n");
    } while (!validateDate(y,m,d));
    sprintf(newDate,"%04d-%02d-%02d",y,m,d);

    fprintf(temp, "%s,%s,%s,%s\n", newName, newEmail, newPhone, newDate);
    printf("Record updated!\n");
    return 1;
}

//ลบข้อมูลผู้เข้าร่วม
int delete_info(char *line)
{
    printf("Record deleted: %s\n", line);
    return 1; // ไม่เขียนลง temp.csv
}


//ค้นหาข้อมูลผู้เข้าร่วมสัมมนา
int search_info()
{
    FILE *fp = fopen("Seminar.csv","r");
    if (!fp) { printf("Cannot open file!\n"); return 0; }

    char keyword [50];
    char line [200];
    char records[100][200]; // เก็บ record ที่ตรง keyword
    int count = 0;

    printf("Enter Name/Email/Phone/Date to search: ");
    scanf(" %[^\n]", keyword);

    fgets(line,sizeof(line),fp); // อ่าน header
    char header[200];
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
        char tmp[200];
        strcpy(tmp,records[i]);
        char name[50]="",email[50]="",phone[20]="",date[20]="";
        char *token=strtok(tmp,","); if(token) strcpy(name,token);
        token=strtok(NULL,","); if(token) strcpy(email,token);
        token=strtok(NULL,","); if(token) strcpy(phone,token);
        token=strtok(NULL,","); if(token) strcpy(date,token);

        printf("\n[%d] Participant Information\n", i+1);
        printf("Name: %s\n", name);
        printf("Email: %s\n", email);
        printf("Phone Number: %s\n", phone);
        printf("Registration Date: %s\n", date);
    }

    int choice,index;
    printf("\nSelect record index to act on (0 = skip): ");
    scanf("%d",&index);
    while(getchar()!='\n');
    if (index <= 0 || index > count) {
        printf("Skip action.\n");
        return 0;
    }

    // สร้าง temp file สำหรับเขียนไฟล์ใหม่
    FILE *temp = fopen("temp.csv","w");
    if (!temp) { printf("Cannot create temp file!\n"); return 0; }
    fprintf(temp,"%s",header);

    for (int i=0;i<count;i++) {
        if (i == index-1) { // record ที่เลือก
            printf("\nWhat do you want to do?\n1.Update\n2.Delete\n3.Skip\nChoice: ");
            scanf("%d",&choice);
            while(getchar()!='\n');

            if (choice==1) { update_info(temp); } // อัพเดท
            else if (choice==2) { delete_info(records[i]); /* ไม่เขียนลง temp */ }
            else { fprintf(temp,"%s",records[i]); }
        } else {
            fprintf(temp,"%s",records[i]);
        }
    }

    // นำ record อื่น ๆ จากไฟล์เก่ามาเขียน
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


//update หน้าเมนู
int update_direct() 
{
    char keyword[50];
    char line[200];
    char records[100][200];
    int count = 0;

    FILE *fp = fopen("Seminar.csv","r");
    if (!fp) { printf("Cannot open file!\n"); return 0; }


    printf("Enter keyword to update (Name/Email/Phone/Date): ");
    scanf(" %[^\n]", keyword);

    fgets(line,sizeof(line),fp); // header
    char header[200]; strcpy(header,line);

    while(fgets(line,sizeof(line),fp)) {
        if (strstr(line,keyword)) {
            strcpy(records[count++],line);
        }
    }
    fclose(fp);

    if (count==0) { printf("No record found!\n"); return 0; }

    printf("\n--- Found %d record(s) ---\n", count);
    for (int i=0;i<count;i++) {
        char tmp[200]; strcpy(tmp,records[i]);
        char name[50]="",email[50]="",phone[20]="",date[20]="";
        char *token=strtok(tmp,","); if(token) strcpy(name,token);
        token=strtok(NULL,","); if(token) strcpy(email,token);
        token=strtok(NULL,","); if(token) strcpy(phone,token);
        token=strtok(NULL,","); if(token) strcpy(date,token);

        printf("\n[%d] Participant Information\n", i+1);
        printf("Name: %s\n", name);
        printf("Email: %s\n", email);
        printf("Phone Number: %s\n", phone);
        printf("Registration Date: %s\n", date);
    }

    int index;
    printf("\nSelect record index to update (0 = skip): ");
    scanf("%d",&index);
    while(getchar()!='\n');
    if (index<=0 || index>count) { printf("Skip update.\n"); return 0; }

    FILE *temp = fopen("temp.csv","w");
    if (!temp) { printf("Cannot create temp file!\n"); return 0; }
    fprintf(temp,"%s",header);

    for (int i=0;i<count;i++) {
        if (i==index-1) update_info(temp);
        else fprintf(temp,"%s",records[i]);
    }

    // นำ record อื่น ๆ จากไฟล์เก่ามาเขียน
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

//ลบหน้าเมนู
int delete_direct() 
{
    char keyword[50];
    char line[200];
    char records[100][200];
    int count = 0;

    FILE *fp = fopen("Seminar.csv","r");
    if (!fp) { printf("Cannot open file!\n"); return 0; }

    printf("Enter keyword to delete (Name/Email/Phone/Date): ");
    scanf(" %[^\n]", keyword);

    fgets(line,sizeof(line),fp); // header
    char header[200]; strcpy(header,line);

    while(fgets(line,sizeof(line),fp)) {
        if (strstr(line,keyword)) {
            strcpy(records[count++],line);
        }
    }
    fclose(fp);

    if (count==0) { printf("No record found!\n"); return 0; }

    printf("\n--- Found %d record(s) ---\n", count);
    for (int i=0;i<count;i++) {
        char tmp[200]; strcpy(tmp,records[i]);
        char name[50]="",email[50]="",phone[20]="",date[20]="";
        char *token=strtok(tmp,","); if(token) strcpy(name,token);
        token=strtok(NULL,","); if(token) strcpy(email,token);
        token=strtok(NULL,","); if(token) strcpy(phone,token);
        token=strtok(NULL,","); if(token) strcpy(date,token);

        printf("\n[%d] Participant Information\n", i+1);
        printf("Name: %s\n", name);
        printf("Email: %s\n", email);
        printf("Phone Number: %s\n", phone);
        printf("Registration Date: %s\n", date);
    }

    int index;
    printf("\nSelect record index to delete (0 = skip): ");
    scanf("%d",&index);
    while(getchar()!='\n');
    if (index<=0 || index>count) { printf("Skip delete.\n"); return 0; }

    FILE *temp = fopen("temp.csv","w");
    if (!temp) { printf("Cannot create temp file!\n"); return 0; }
    fprintf(temp,"%s",header);

    for (int i=0;i<count;i++) {
        if (i==index-1) delete_info(records[i]); // ไม่เขียนลง temp
        else fprintf(temp,"%s",records[i]);
    }

    // นำ record อื่น ๆ จากไฟล์เก่ามาเขียน
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
    printf("File: %p\n",fp);
    printf("Cannot open the file\n");
    return 0;
    }
    char line[200];
    char *token;

    printf("\n=================================================================================\n");
    printf("| %-20s | %-25s | %-12s | %-12s |\n",
           "Name", "Email", "Phone", "Date");
    printf("=================================================================================\n");

    // ข้าม header ไม่ต้องพิมพ์ซ้ำ
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp))
    {
        line[strcspn(line, "\n")] = 0; // ตัด \n ออก

        token = strtok(line, ",");
        char name[50], email[50], phone[20], date[20];

        if (token) strcpy(name, token);
        token = strtok(NULL, ",");
        if (token) strcpy(email, token);
        token = strtok(NULL, ",");
        if (token) strcpy(phone, token);
        token = strtok(NULL, ",");
        if (token) strcpy(date, token);

        printf("| %-20s | %-25s | %-12s | %-12s |\n",
               name, email, phone, date);
    }

    printf("=================================================================================\n");

    fclose(fp);
    return 1;
}

// UNIT TEST
void unit_test(){

}

// E2E TEST
void e2e_test(){

}

//แสดงเมนู 
int display_menu()
{
    int choice ;
    do {
        printf("\n Welcome to the seminar! \n");
        printf("\n========= MENU ========== \n");
        printf("| 1. Show all information\n");
        printf("| 2. Add participants\n");
        printf("| 3. Search for participants information\n");
        printf("| 4. Update participant \n");
        printf("| 5. Delete participant \n");
        printf("| 6. UNIT TEST \n");
        printf("| 7. E2E TEST \n");
        printf("| 8. Exit\n");
        printf("---> Please select an option: ");
        scanf("%d", &choice);
        while(getchar()!='\n');

        switch (choice) {
            case 1: display_all(); break;
            case 2: add_info(); break;
            case 3: search_info(); break;
            case 4: update_direct(); break;
            case 5: delete_direct(); break;
            case 6: unit_test(); break;
            case 7: e2e_test(); break;
            case 8: printf("\nExit program!\n"); break; 
            default: printf("\nInvalid choice!\n");
        }
    } while (choice != 8);
}

int main()
{
    createfile();
    readCSV();
    display_menu();
    return 0;
}


