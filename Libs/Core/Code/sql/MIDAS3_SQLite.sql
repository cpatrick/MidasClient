-------------------------------------------------------------------------------
-- Copyright (c) Kitware Inc. 28 Corporate Drive. All rights reserved.
-- Clifton Park, NY, 12065, USA.
--
-- See Copyright.txt for details.
-- This software is distributed WITHOUT ANY WARRANTY; without even
-- the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
-- PURPOSE.  See the above copyright notices for more information.
-------------------------------------------------------------------------------

CREATE TABLE auth_profile (
  profile_name character varying(64) PRIMARY KEY,
  eperson character varying(64),
  apikey character varying(40),
  app_name character varying(256),
  url character varying(512),
  root_dir character varying(512)
);

CREATE TABLE app_settings (
  key character varying(512) PRIMARY KEY,
  value character varying(512)
);

INSERT INTO app_settings (key, value) VALUES ('refresh_setting', '0');
INSERT INTO app_settings (key, value) VALUES ('refresh_interval', '5');

CREATE TABLE version (
  name character varying(64),
  major integer,
  minor integer,
  patch integer
);

CREATE TABLE folder (
  folder_id integer PRIMARY KEY AUTOINCREMENT,
  parent_id bigint NOT NULL DEFAULT '0',
  name character varying(255) NOT NULL,
  description text NOT NULL,
  date_update timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  uuid character varying(255) DEFAULT '',
  date_creation timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  path character varying(512)
);
CREATE INDEX folder_parent_id_idx ON folder (parent_id);

CREATE TABLE item (
  item_id integer PRIMARY KEY AUTOINCREMENT,
  name character varying(250) NOT NULL,
  date_update timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  description character varying(20) NOT NULL,
  sizebytes bigint NOT NULL DEFAULT '0',
  uuid character varying(255) DEFAULT '',
  date_creation timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  path character varying(512)
);

CREATE TABLE IF NOT EXISTS item2folder (
  item_id bigint NOT NULL,
  folder_id bigint NOT NULL
);
CREATE INDEX item2folder_folder_id_idx ON item2folder (folder_id);

CREATE TABLE bitstream (
  bitstream_id integer PRIMARY KEY AUTOINCREMENT,
  item_id bigint NOT NULL DEFAULT '0',
  name character varying(250) NOT NULL,
  date_update timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  last_modified bigint,
  sizebytes bigint NOT NULL DEFAULT '0',
  uuid character varying(255) DEFAULT '',
  date_creation timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  checksum character varying(50),
  path character varying(512)
);
CREATE INDEX bitstream_checksum_idx ON bitstream (checksum);

-- --------------------------------------------------------

-- Uploads canceled/interrupted partway through
--CREATE TABLE partial_upload (
--  id integer PRIMARY KEY AUTOINCREMENT,
--  bitstream_id integer,
--  uploadtoken character varying(512),
--  user_id integer,
--  item_id integer
--);

-- Downloads canceled/interrupted partway through
--CREATE TABLE partial_download (
--  id integer PRIMARY KEY AUTOINCREMENT,
--  uuid character varying(60),
--  path character varying(512),
--  item_id integer
--);

