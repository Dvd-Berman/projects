package il.co.ilrd.gatewayserver;

import java.sql.SQLException;

public interface FactoryCommand {
	public String run(Object data, DatabaseManagementInterface DbManagemaent) throws SQLException;
}
