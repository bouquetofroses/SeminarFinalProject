#include<stdio.h>
//ระบบจัดการข้อมูลผู้เข้าร่วมสัมมนา

//บันทึกข้อมูลผู้เข้าร่วมสัมมนาลงไฟล์ CSV
int saveCSV()
{
    FILE *fp =fopen("Seminar.csv","w");
    fprintf(fp, "ParticipantName,Email,PhoneNumber,RegistrationDate\n");
    if (fp==NULL){
        printf("Cannot creat file\n");
        return 0;
    }
    fclose (fp);
    printf("[Data saved to CSV\n]");
}

//อ่านข้อมูลผู้เข้าร่วมสัมมนาจากไฟล์ CSV
int readCSV()
{
    FILE *fp =fopen("Seminar.csv","r");
    //ตรวจสอบการเปิดไฟล์
    if (fp == NULL)
    {
        printf("File not found\n");
    }
}

//เพิ่มข้อมูลผู้เข้าร่วมใหม่
int add_info()
{

    saveCSV();
    return 0 ;
}

//ค้นหาข้อมูลผู้เข้าร่วมสัมมนา
int search_info()
{

}

//อัพเดทข้อมูลผู้เข้าร่วม
int update_info()
{

}

//ลบข้อมูลผู้เข้าร่วม
int delete_info()
{

}

//แดสงข้อมูลทั้งหมด
int display_all()
{

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
    display_menu();
    return 0;
}


