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

CREATE TABLE bitstream (
    bitstream_id integer PRIMARY KEY AUTOINCREMENT,
    bitstream_format_id integer,
    name character varying(256),
    size_bytes bigint,
    checksum character varying(64),
    checksum_algorithm character varying(32),
    description text,
    user_format_description text,
    source character varying(256),
    internal_id character varying(512),
    location smallint DEFAULT 0,
    deleted boolean,
    last_modified bigint,
    store_number integer,
    sequence_id integer
);

CREATE TABLE collection (
    collection_id integer PRIMARY KEY AUTOINCREMENT,
    name character varying(128),
    short_description character varying(512),
    introductory_text text,
    logo_bitstream_id integer,
    template_item_id integer,
    provenance_description text,
    license text,
    copyright_text text,
    side_bar_text text,
    workflow_step_1 integer,
    workflow_step_2 integer,
    workflow_step_3 integer,
    submitter integer,
    admin integer
);

CREATE TABLE collection2item (
    id integer PRIMARY KEY AUTOINCREMENT,
    collection_id integer,
    item_id integer
);

CREATE TABLE community (
    community_id integer PRIMARY KEY AUTOINCREMENT,
    name character varying(128),
    short_description character varying(512),
    introductory_text text,
    logo_bitstream_id integer,
    copyright_text text,
    side_bar_text text
);

CREATE TABLE community2collection (
    id integer PRIMARY KEY AUTOINCREMENT,
    community_id integer,
    collection_id integer
);

CREATE TABLE community2community (
    id integer PRIMARY KEY AUTOINCREMENT,
    parent_comm_id integer,
    child_comm_id integer
);

CREATE TABLE metadatafieldregistry (
    metadata_field_id integer PRIMARY KEY AUTOINCREMENT,
    metadata_schema_id integer NOT NULL,
    element character varying(64),
    qualifier character varying(64),
    scope_note text
);

CREATE TABLE metadatavalue (
    metadata_value_id integer PRIMARY KEY AUTOINCREMENT,
    item_id integer,
    metadata_field_id integer,
    text_value text,
    text_lang character varying(24),
    place integer
);

CREATE TABLE resource_uuid (
    resource_type_id integer,
    resource_id integer,
    path character varying(512),
    server_parent integer,
    uuid character varying(60) PRIMARY KEY
);

-- A list of resources flagged as dirty
-- action is the action that marked the resource dirty (modified, added, or removed)
CREATE TABLE dirty_resource (
    uuid character varying(60) PRIMARY KEY,
    action smallint
);

CREATE TABLE item (
    item_id integer PRIMARY KEY AUTOINCREMENT,
    title character varying(128),
    submitter_id integer,
    in_archive boolean,
    withdrawn boolean,
    last_modified timestamp with time zone,
    owning_collection integer
);

CREATE TABLE item2bitstream (
    id integer PRIMARY KEY AUTOINCREMENT,
    item_id integer,
    bitstream_id integer
);

-- List of item metadata fields copied from MIDAS server
INSERT INTO metadatafieldregistry VALUES (1, 1, 'contributor', NULL, 'A person, organization, or service responsible for the content of the resource.  Catch-all for unspecified contributors.');
INSERT INTO metadatafieldregistry VALUES (2, 1, 'contributor', 'advisor', 'Use primarily for thesis advisor.');
INSERT INTO metadatafieldregistry VALUES (3, 1, 'contributor', 'author', NULL);
INSERT INTO metadatafieldregistry VALUES (4, 1, 'contributor', 'editor', NULL);
INSERT INTO metadatafieldregistry VALUES (5, 1, 'contributor', 'illustrator', NULL);
INSERT INTO metadatafieldregistry VALUES (6, 1, 'contributor', 'other', NULL);
INSERT INTO metadatafieldregistry VALUES (7, 1, 'coverage', 'spatial', 'Spatial characteristics of content.');
INSERT INTO metadatafieldregistry VALUES (8, 1, 'coverage', 'temporal', 'Temporal characteristics of content.');
INSERT INTO metadatafieldregistry VALUES (9, 1, 'creator', NULL, 'Do not use only for harvested metadata.');
INSERT INTO metadatafieldregistry VALUES (10, 1, 'date', NULL, 'Use qualified form if possible.');
INSERT INTO metadatafieldregistry VALUES (11, 1, 'date', 'accessioned', 'Date DSpace takes possession of item.');
INSERT INTO metadatafieldregistry VALUES (12, 1, 'date', 'available', 'Date or date range item became available to the public.');
INSERT INTO metadatafieldregistry VALUES (13, 1, 'date', 'copyright', 'Date of copyright.');
INSERT INTO metadatafieldregistry VALUES (14, 1, 'date', 'created', 'Date of creation or manufacture of intellectual content if different from date.issued.');
INSERT INTO metadatafieldregistry VALUES (15, 1, 'date', 'issued', 'Date of publication or distribution.');
INSERT INTO metadatafieldregistry VALUES (16, 1, 'date', 'submitted', 'Recommend for theses/dissertations.');
INSERT INTO metadatafieldregistry VALUES (17, 1, 'identifier', NULL, 'Catch-all for unambiguous identifiers not defined by
    qualified form use identifier.other for a known identifier common
    to a local collection instead of unqualified form.');
INSERT INTO metadatafieldregistry VALUES (18, 1, 'identifier', 'citation', 'Human-readable, standard bibliographic citation
    of non-DSpace format of this item');
INSERT INTO metadatafieldregistry VALUES (19, 1, 'identifier', 'govdoc', 'A government document number');
INSERT INTO metadatafieldregistry VALUES (20, 1, 'identifier', 'isbn', 'International Standard Book Number');
INSERT INTO metadatafieldregistry VALUES (21, 1, 'identifier', 'issn', 'International Standard Serial Number');
INSERT INTO metadatafieldregistry VALUES (22, 1, 'identifier', 'sici', 'Serial Item and Contribution Identifier');
INSERT INTO metadatafieldregistry VALUES (23, 1, 'identifier', 'ismn', 'International Standard Music Number');
INSERT INTO metadatafieldregistry VALUES (24, 1, 'identifier', 'other', 'A known identifier type common to a local collection.');
INSERT INTO metadatafieldregistry VALUES (25, 1, 'identifier', 'uri', 'Uniform Resource Identifier');
INSERT INTO metadatafieldregistry VALUES (26, 1, 'description', NULL, 'Catch-all for any description not defined by qualifiers.');
INSERT INTO metadatafieldregistry VALUES (27, 1, 'description', 'abstract', 'Abstract or summary.');
INSERT INTO metadatafieldregistry VALUES (28, 1, 'description', 'provenance', 'The history of custody of the item since its creation, including any changes successive custodians made to it.');
INSERT INTO metadatafieldregistry VALUES (29, 1, 'description', 'sponsorship', 'Information about sponsoring agencies, individuals, or
    contractual arrangements for the item.');
INSERT INTO metadatafieldregistry VALUES (30, 1, 'description', 'statementofresponsibility', 'To preserve statement of responsibility from MARC records.');
INSERT INTO metadatafieldregistry VALUES (31, 1, 'description', 'tableofcontents', 'A table of contents for a given item.');
INSERT INTO metadatafieldregistry VALUES (32, 1, 'description', 'uri', 'Uniform Resource Identifier pointing to description of
    this item.');
INSERT INTO metadatafieldregistry VALUES (33, 1, 'format', NULL, 'Catch-all for any format information not defined by qualifiers.');
INSERT INTO metadatafieldregistry VALUES (34, 1, 'format', 'extent', 'Size or duration.');
INSERT INTO metadatafieldregistry VALUES (35, 1, 'format', 'medium', 'Physical medium.');
INSERT INTO metadatafieldregistry VALUES (36, 1, 'format', 'mimetype', 'Registered MIME type identifiers.');
INSERT INTO metadatafieldregistry VALUES (37, 1, 'language', NULL, 'Catch-all for non-ISO forms of the language of the
    item, accommodating harvested values.');
INSERT INTO metadatafieldregistry VALUES (38, 1, 'language', 'iso', 'Current ISO standard for language of intellectual content, including country codes (e.g. "en_US").');
INSERT INTO metadatafieldregistry VALUES (39, 1, 'publisher', NULL, 'Entity responsible for publication, distribution, or imprint.');
INSERT INTO metadatafieldregistry VALUES (40, 1, 'relation', NULL, 'Catch-all for references to other related items.');
INSERT INTO metadatafieldregistry VALUES (41, 1, 'relation', 'isformatof', 'References additional physical form.');
INSERT INTO metadatafieldregistry VALUES (42, 1, 'relation', 'ispartof', 'References physically or logically containing item.');
INSERT INTO metadatafieldregistry VALUES (43, 1, 'relation', 'ispartofseries', 'Series name and number within that series, if available.');
INSERT INTO metadatafieldregistry VALUES (44, 1, 'relation', 'haspart', 'References physically or logically contained item.');
INSERT INTO metadatafieldregistry VALUES (45, 1, 'relation', 'isversionof', 'References earlier version.');
INSERT INTO metadatafieldregistry VALUES (46, 1, 'relation', 'hasversion', 'References later version.');
INSERT INTO metadatafieldregistry VALUES (47, 1, 'relation', 'isbasedon', 'References source.');
INSERT INTO metadatafieldregistry VALUES (48, 1, 'relation', 'isreferencedby', 'Pointed to by referenced resource.');
INSERT INTO metadatafieldregistry VALUES (49, 1, 'relation', 'requires', 'Referenced resource is required to support function,
    delivery, or coherence of item.');
INSERT INTO metadatafieldregistry VALUES (50, 1, 'relation', 'replaces', 'References preceeding item.');
INSERT INTO metadatafieldregistry VALUES (51, 1, 'relation', 'isreplacedby', 'References succeeding item.');
INSERT INTO metadatafieldregistry VALUES (52, 1, 'relation', 'uri', 'References Uniform Resource Identifier for related item.');
INSERT INTO metadatafieldregistry VALUES (53, 1, 'rights', NULL, 'Terms governing use and reproduction.');
INSERT INTO metadatafieldregistry VALUES (54, 1, 'rights', 'uri', 'References terms governing use and reproduction.');
INSERT INTO metadatafieldregistry VALUES (55, 1, 'source', NULL, 'Do not use only for harvested metadata.');
INSERT INTO metadatafieldregistry VALUES (56, 1, 'source', 'uri', 'Do not use only for harvested metadata.');
INSERT INTO metadatafieldregistry VALUES (57, 1, 'subject', NULL, 'Uncontrolled index term.');
INSERT INTO metadatafieldregistry VALUES (58, 1, 'subject', 'classification', 'Catch-all for value from local classification system global classification systems will receive specific qualifier');
INSERT INTO metadatafieldregistry VALUES (59, 1, 'subject', 'ddc', 'Dewey Decimal Classification Number');
INSERT INTO metadatafieldregistry VALUES (60, 1, 'subject', 'lcc', 'Library of Congress Classification Number');
INSERT INTO metadatafieldregistry VALUES (61, 1, 'subject', 'lcsh', 'Library of Congress Subject Headings');
INSERT INTO metadatafieldregistry VALUES (62, 1, 'subject', 'mesh', 'MEdical Subject Headings');
INSERT INTO metadatafieldregistry VALUES (63, 1, 'subject', 'other', 'Local controlled vocabulary global vocabularies will receive specific qualifier.');
INSERT INTO metadatafieldregistry VALUES (64, 1, 'title', NULL, 'Title statement/title proper.');
INSERT INTO metadatafieldregistry VALUES (65, 1, 'title', 'alternative', 'Varying (or substitute) form of title proper appearing in item, e.g. abbreviation or translation');
INSERT INTO metadatafieldregistry VALUES (66, 1, 'type', NULL, 'Nature or genre of content.');
INSERT INTO metadatafieldregistry VALUES (67, 1, 'description', 'tag', 'Tag for search');
INSERT INTO metadatafieldregistry VALUES (68, 1, 'subject', 'ocis', 'Optics Classification and Indexing Scheme');
INSERT INTO metadatafieldregistry VALUES (69, 1, 'description', 'volumenumber', 'Volume Number');
INSERT INTO metadatafieldregistry VALUES (70, 1, 'description', 'issuenumber', 'Issue Number');
INSERT INTO metadatafieldregistry VALUES (71, 1, 'description', 'pagesfirst', 'First Page');
INSERT INTO metadatafieldregistry VALUES (72, 1, 'description', 'pageslast', 'Last Page');
INSERT INTO metadatafieldregistry VALUES (73, 1, 'description', 'pagesextent', 'Number of pages');
INSERT INTO metadatafieldregistry VALUES (74, 1, 'identifier', 'doi', 'Digital Object Identifier');
INSERT INTO metadatafieldregistry VALUES (75, 1, 'identifier', 'pubmed', 'PubMed id');

CREATE INDEX collection2item_collection_idx ON collection2item (collection_id);
CREATE INDEX collection2item_item_id_idx ON collection2item (item_id);
CREATE INDEX community2collection_collection_id_idx ON community2collection (collection_id);
CREATE INDEX community2collection_community_id_idx ON community2collection (community_id);
CREATE INDEX metadatavalue_item_idx ON metadatavalue (item_id);
CREATE INDEX metadatavalue_item_idx2 ON metadatavalue (item_id, metadata_field_id);
