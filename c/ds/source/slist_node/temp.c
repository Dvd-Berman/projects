/* Testing CreateNode */ 	
	for (i = 0; i < 5; ++i)
	{
		nodes[i] = CreateNode(int_data_arr + i, next);
		CheckResult(NULL != nodes[i], "TestCreateNode");
	}
	
	/* Testing InsertAfter */
	for (i = 0; i < 4; ++i)
	{
		new_node = InsertAfter(nodes[i], nodes[i + 1]);
		CheckResult(IsNodesEqual(nodes[i+1], new_node), "TestInsertAfterNode");	
	}
	
	/* Testing FreeAll */
	FreeAll(nodes[0]);
	
	/* creating new nodes for second test */
	for (i = 0; i < 5; ++i)
	{
		nodes[i] = CreateNode(int_data_arr + i, next);
	}
	/* creating new node list for second test */
	for (i = 0; i < 4; ++i)
	{
		new_node = InsertAfter(nodes[i], nodes[i + 1]);
	}
	
	/* Testing RemoveAfer */	
	for (i = 3; 0 < i; --i)
	{
		new_node = RemoveAfter(nodes[i - 1]);
		CheckResult(IsNodesEqual(nodes[i], new_node), "TestRemoveAfterNode");
		free(new_node);
	}
	
	
	free(nodes[0]);
	free(nodes[4]);
	
	/* creating new nodes for third test */
	for (i = 0; i < 5; ++i)
	{
		nodes[i] = CreateNode(int_data_arr + i, next);
	}
	/* Testing Insert */
	for (i = 4; 0 < i; --i)
	{
		new_node = Insert(nodes[i], nodes[i - 1]);
		CheckResult(IsNodesEqual(nodes[i-1], new_node), "TestInsert");
	}
	FreeAll(nodes[0]);
	
