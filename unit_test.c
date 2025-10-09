#include "project.h"

void run_full_unit_test_suite() {
    printf("\n=== RUNNING FULL UNIT TEST SUITE ===\n");

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