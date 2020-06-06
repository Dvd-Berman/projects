#ifndef __TESTS_H__
#define __TESTS_H__


/* Open test file. */
void OpenTestFile(char *file_name);

/* Print the result of tests. */
void CheckResult(int test, const char *func_name);

/* Print the test result. */
void PrintTsetResult(int result, const char *str);

void TestResult(void);

#endif /* __TESTS_H__ */
