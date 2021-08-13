/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */

CREATE TABLE IF NOT EXISTS ADMIN (
	name TEXT NOT NULL,
	activated INTEGER NOT NULL,

	PRIMARY KEY(name)
);

CREATE TABLE IF NOT EXISTS POLICY_DEFINITION (
	name TEXT NOT NULL,
	ivalue TEXT NOT NULL,

	PRIMARY KEY(name)
);

CREATE TABLE IF NOT EXISTS POLICY_MANAGED (
	admin TEXT NOT NULL,
	policy TEXT NOT NULL,
	value TEXT NOT NULL,

	PRIMARY KEY(admin, policy),
	FOREIGN KEY(admin) REFERENCES ADMIN(name),
	FOREIGN KEY(policy) REFERENCES POLICY_DEFINITION(name)
);

CREATE TRIGGER IF NOT EXISTS ACTIVATE AFTER INSERT ON ADMIN
FOR EACH ROW
BEGIN
	INSERT INTO POLICY_MANAGED(admin, policy, value)
		SELECT NEW.name, POLICY_DEFINITION.name, POLICY_DEFINITION.ivalue
		FROM POLICY_DEFINITION;
END;

CREATE TRIGGER IF NOT EXISTS DEACTIVATE AFTER DELETE ON ADMIN
FOR EACH ROW
BEGIN
	DELETE FROM POLICY_MANAGED WHERE POLICY_MANAGED.admin = OLD.name;
END;
