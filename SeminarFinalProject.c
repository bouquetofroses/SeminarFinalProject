#include<stdio.h>
#include<string.h>
//ระบบจัดการข้อมูลผู้เข้าร่วมสัมมนา


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

//เพิ่มข้อมูลผู้เข้าร่วมใหม่
int add_info()
{
    FILE *fp;
    char name[50];
    char email[50];
    char phone[20];
    char regDate[20];

    printf("Please enter your name: ");
    scanf(" %[^\n]", name);
    printf("Please enter your email: ");
    scanf(" %[^\n]", email);
    printf("Please enter your phone number: ");
    scanf(" %[^\n]", phone);
    printf("Please enter the registration date (YYYY-MM-DD): ");
    scanf(" %[^\n]", regDate);

    saveCSV(name,email,phone,regDate);
    printf("Data added successfully!\n");

    return 0 ;
}

//ค้นหาข้อมูลผู้เข้าร่วมสัมมนา
int search_info()
{
    FILE *fp = fopen("Seminar.csv","r");
    if (fp == NULL)
    {
    printf("File: %p\n",fp);
    printf("Cannot open the file\n");
    return 0;
    }

    char keyword [50];
    char line [200];
    int found = 0;

    printf("Enter your Name/Email/PhoneNumber/RegistrationDate  : ");
    scanf("%[^\n]",keyword);

    while (fgets(line, sizeof(line),fp))
    {
        if (strstr(line, keyword))
        {
            printf("%s",line);
            found = 1;
        }
    }
    if (!found)
    {
        printf("\n----> Data not found!, Please try seraching again\n");
        printf("\n----> You can search by name, email, phone number, or registration date\n");
    }
    fclose(fp);
    return 1;
}

//อัพเดทข้อมูลผู้เข้าร่วม
int update_info()
{

}

//ลบข้อมูลผู้เข้าร่วม
int delete_info()
{

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
    char line[100];
    while(fgets(line,sizeof(line),fp)!=NULL){
        printf("%s",line);
    }
    fclose(fp);
    return 1;
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
        printf("| 3. Search for paticipants information\n");
        printf("| 4. Update paticipants information\n");
        printf("| 5. Delete paticipants information\n");
        printf("| 6. Exit\n");
        printf("---> Please select an option: ");
        scanf("%d", &choice);
        while(getchar()!='\n');

        switch (choice) {
            case 1: display_all(); break; //แสดงข้อมูลทั้งหมด
            case 2: add_info(); break; //เพิ่มข้อมูล
            case 3: search_info(); break; //ค้นหาข้อมูล
            case 4: update_info(); break; //แก้ไขข้อมูล
            case 5: delete_info(); break; //ลบข้อมูล
            case 6: printf("\nExit program!\n"); break; 
            default: printf("\nInvalid choice!\n");
        }
    } while (choice != 6);
}




int main()
{
    readCSV();
    display_menu();
    return 0;
}


