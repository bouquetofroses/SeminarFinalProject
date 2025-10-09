#include "project.h"

void run_full_e2e_test_suite() {
    printf("\n=== RUNNING FULL E2E TEST SUITE (ADMIN & PARTICIPANT) ===\n");

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
