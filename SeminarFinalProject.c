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
    return 0;
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
    
    return 0;
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
    FILE *temp = fopen("temp.csv","w");
    if (fp == NULL || temp == NULL)
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
            found = 1;
            printf("\nFound record : %s",line);

            int choice;
            printf("\nWhat do you want to do?\n");
            printf("| 1. Update this record\n");
            printf("| 2. Delete this record\n");
            printf("| 3. Do nothing\n");
            printf("---> Enter choice : ");
            scanf("%d",&choice);

            if (choice == 1)//อัพเดต
            {
                char newName[50];
                char newEmail[50];
                char newPhone[20];
                char newDate[20];
                printf("Enter new name : ");
                scanf(" %[^\n]", newName);
                printf("Enter new email: ");
                scanf(" %[^\n]", newEmail);
                printf("Enter new phone: ");
                scanf(" %[^\n]", newPhone);
                printf("Enter new date: ");
                scanf(" %[^\n]", newDate);

                fprintf(temp, "%s,%s,%s,%s\n", newName, newEmail, newPhone, newDate);
                printf("Record update!\n");
            }
            else if (choice == 2)//ลบ
            {
                printf("Record deleted!\n");
            }
            else 
            {
                fprintf(temp,"%s",line);
            }
        }
        else {
            fprintf(temp, "%s",line);
        }
    }

    fclose(fp);
    fclose(temp);

    remove("Seminar.csv");
    rename("temp.csv", "Seminar.csv");

    if (!found)
    {
        printf("\n----> Data not found!, Please try seraching again\n");
        printf("\n----> You can search by name, email, phone number, or registration date\n");
    }
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
        printf("| 3. Search for participants information\n");
        printf("| 4. Exit\n");
        printf("---> Please select an option: ");
        scanf("%d", &choice);
        while(getchar()!='\n');

        switch (choice) {
            case 1: display_all(); break; //แสดงข้อมูลทั้งหมด
            case 2: add_info(); break; //เพิ่มข้อมูล
            case 3: search_info(); break; //ค้นหาข้อมูล
            case 4: printf("\nExit program!\n"); break; 
            default: printf("\nInvalid choice!\n");
        }
    } while (choice != 4);
}

int main()
{
    readCSV();
    display_menu();
    return 0;
}


