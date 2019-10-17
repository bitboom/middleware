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

CREATE TABLE IF NOT EXISTS admin (
    id        INTEGER PRIMARY KEY AUTOINCREMENT,
    pkg       TEXT NOT NULL,
    uid       INTEGER,
    key       TEXT,
    removable INTEGER
);

CREATE TABLE IF NOT EXISTS managed_policy (
    id        INTEGER PRIMARY KEY AUTOINCREMENT,
    aid       INTEGER,
    pid       INTEGER,
    value     INTEGER 
);

CREATE TABLE IF NOT EXISTS policy_definition (
    id        INTEGER PRIMARY KEY AUTOINCREMENT,
    scope     INTEGER,
    name      TEXT NOT NULL,
    ivalue    INTEGER
);

CREATE TRIGGER IF NOT EXISTS prepare_storage AFTER INSERT ON admin
FOR EACH ROW
BEGIN
	INSERT INTO managed_policy(aid, pid, value)
		SELECT NEW.ID, policy_definition.id, policy_definition.ivalue
		FROM policy_definition;
END;

CREATE TRIGGER IF NOT EXISTS remove_storage AFTER DELETE ON admin
FOR EACH ROW
BEGIN
	DELETE FROM managed_policy WHERE managed_policy.aid = OLD.ID;
END;
