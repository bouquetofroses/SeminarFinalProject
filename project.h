#ifndef PROJECT_H
#define PROJECT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

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
int validatePhone(char *phone);
int isPhoneDuplicate(const char *phone); // <-- เพิ่มบรรทัดนี้
int isPhoneDuplicateExcept(const char *phone, const char *except_id); // <-- เพิ่มบรรทัดนี้
int validateDate(int y, int m, int d);

extern int test_case_count;
extern int test_fail_count;

void print_test_result(const char *desc, int passed);
void setup_test_csv_full();
void restore_csv_full();
void backup_csv_full();
int participant_edit_self_sim(const char *email, const char *phone,const char *newName, const char *newEmail, const char *newPhone);
int simulate_update_record(const char *targetEmail, const char *newEmail, const char *newPhone, const char *newName);
void run_full_unit_test_suite();

int check_record_status(const char *id_to_find);
int simulate_add_record(const char *name, const char *email, const char *phone, char *out_id, size_t out_sz);
int simulate_delete_record(const char *targetID);
void run_full_e2e_test_suite();

#endif