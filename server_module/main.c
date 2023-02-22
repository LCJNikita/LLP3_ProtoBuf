#include "graph.h"
#include <string.h>

// ��������� ����� �� � ���������� �� � ����
//int main() {
//
//	struct GraphDB db;
//    db.rowCount = 0;
//    db.schemeCount = 0;
//    db.rows = NULL;
//    db.scheme = NULL;
//
//    int schemeCount = 4;
//
//    struct Column scheme[schemeCount];
//    scheme[0].type = INT32;
//    strncpy(scheme[0].name, "Age\0", MAX_NAME_LENGTH);
//
//    scheme[1].type = FLOAT;
//    strncpy(scheme[1].name, "Balance\0", MAX_NAME_LENGTH);
//
//    scheme[2].type = CHAR_PTR;
//    strncpy(scheme[2].name, "Name\0", MAX_NAME_LENGTH);
//
//    scheme[3].type = BOOL;
//    strncpy(scheme[3].name, "isAdult\0", MAX_NAME_LENGTH);
//
//    setScheme(&db, scheme, schemeCount);
//
//    saveHeaderStructToFile(&db, "data.bin");
//
//    return 0;
//}

// ������ ������ ��
//int main() {
//
//	struct GraphDB db;
//    loadHeaderStructFromFile(&db, "data.bin");
//    printHeaderGraphDB(&db);
//
//    return 0;
//}



// �������� ������ row
//int main() {
//	
//	struct GraphDB db;
//   	loadHeaderStructFromFile(&db, "data.bin");
//
//	struct Row row;
//	parseAndSetRow(&db, "10000,228.56,nikita aboba,true", &row);
//	printRow(&db, &row);
//    
//    return 0;
//}

// ���������� row � ����
//int main() {
//	
//	struct GraphDB db;
//   	loadHeaderStructFromFile(&db, "data.bin");
//   	
//   	struct Row row;
//	parseAndSetRow(&db, "222,131.12,thelastofus,false", &row);
//
////	printRow(&db, &row);
//	
//	printf("file size BEFORE ADD %d\n", (int)getFileSize("data.bin"));
//	
//	addRowToFile("data.bin", &row);
//
//	printf("file size AFTER ADD %d\n", (int)getFileSize("data.bin"));
//    
//    return 0;
//}

// ������ ������������� ������� findRowById
//int main() {
//	
//	struct GraphDB db;
//   	loadHeaderStructFromFile(&db, "data.bin");
//   	printHeaderGraphDB(&db);
//   	
//   	findRowById("data.bin", 8);
//    
//    return 0;
//}

// ������� ������ row �� �����
//int main() {
//
//	iterateByEachRow("data.bin");
//
//    return 0;
//}

// ������ ������������� ������� createRow
//int main() {
//	
//	bool createRowResult1 = createRow("data.bin", "10000,228.56,nikita123,true");
//	if (createRowResult1) {
//		printf("Adding createRowResult1 success\n");
//	} else {
//		printf("Adding createRowResult1 failure\n");
//	}
//	
//	bool createRowResult2 = createRow("data.bin", "123, kek");
//	if (createRowResult2) {
//		printf("Adding createRowResult2 success\n");
//	} else {
//		printf("Adding createRowResult2 failure\n");
//	}
//    
//    return 0;
//}

// ������ ������������� findRows
//int main() {
//	
//	iterateByEachRow("data.bin");
//	printf("\n");
//	
//	findRows("data.bin", "isAdult", "false");
//    
//    return 0;
//}

// ������ ������������� updateRowById
//int main() {
//	
//	iterateByEachRow("data.bin");
//	printf("\n");
//	
////	updateRowById("data.bin", "Name", "new_nikita_name", 7);
//    
//    return 0;
//}


































