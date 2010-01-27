-------------------------------------------------------------------------------
-- Program:   MIDAS Server
-- Language:  PHP/HTML/Java/Javascript/SQL
-- Date:      $Date: 2010-01-26 15:56:46 -0500 (Tue, 26 Jan 2010) $
-- Version:   $Revision: 1259 $
-- 
-- Copyright (c) Kitware Inc. 28 Corporate Drive. All rights reserved.
-- Clifton Park, NY, 12065, USA.
-- 
-- See Copyright.txt for details.
-- This software is distributed WITHOUT ANY WARRANTY; without even
-- the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
-- PURPOSE.  See the above copyright notices for more information.
-------------------------------------------------------------------------------

--
-- Name: bitstream; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE version(
    name text,
    major integer,
    minor integer,
    patch integer
);

INSERT INTO version(name,major,minor,patch) VALUES ('midas','2','5','0');


--------==== 2.0.2 to 2.0.3 ===----------
--- Create the cart table ---
CREATE TABLE cart (
    cart_id integer PRIMARY KEY AUTOINCREMENT,
    name character varying(256),
    description text,
    modality_id integer,
    creation_date date
);

CREATE INDEX cart_id_idx ON cart (cart_id);

--- Create the cart2eperson table ---

CREATE TABLE cart2eperson (
  cart_id integer NOT NULL,
  eperson_id integer NOT NULL
);

CREATE TABLE resource_uuid (
    resource_type smallint,
    resource_id integer,
    uuid character varying(512) PRIMARY KEY,
    location smallint DEFAULT 0
);

CREATE INDEX cart2eperson_cartid_idx ON cart2eperson (cart_id);
CREATE INDEX cart2eperson_epersonid_idx ON cart2eperson (eperson_id);

--- Create the cart2item table ---

CREATE TABLE cart2item (
  cart_id integer NOT NULL,
  item_id integer NOT NULL
);

CREATE INDEX cart2item_cartid_idx ON cart2item (cart_id);
CREATE INDEX cart2item_bitstreamid_idx ON cart2item (item_id);


--- Create the cartgroup table ---

CREATE TABLE cartgroup (
    cartgroup_id integer PRIMARY KEY AUTOINCREMENT,
    name character varying(256),
    description text,
    creation_date date
);

CREATE INDEX cartgroup_id_idx ON cartgroup (cartgroup_id);

--- Create the cartgroup2cart table ---

CREATE TABLE cartgroup2cart (
  cartgroup_id integer NOT NULL,
  cart_id integer NOT NULL
);

CREATE INDEX cartgroup2cart_cartid_idx ON cartgroup2cart (cartgroup_id);
CREATE INDEX cartgroup2cart_cartgroupid_idx ON cartgroup2cart (cart_id);

--- Create the cart2cart table ---

CREATE TABLE cart2cart (
  cart1_id integer NOT NULL,
  cart2_id integer NOT NULL
);


--- Create the modality table ---

CREATE TABLE modality (
  modality_id integer PRIMARY KEY AUTOINCREMENT,
  name character varying(256),
  description text,
  parentid integer
);

INSERT INTO modality  VALUES ('1','Unkown','Unsupported modality format','0');
INSERT INTO modality  VALUES ('2','MR','Magnetic Resonnance Imaging','0');
INSERT INTO modality  VALUES ('3','CT','Computer Tomography','0');
INSERT INTO modality  VALUES ('4','US','Ultrasound','0');
INSERT INTO modality  VALUES ('5','T1','T1','1');
INSERT INTO modality  VALUES ('6','T2','T2','1');

-----------------------------------------


CREATE TABLE midas_itemstats(
    itemid integer,
    downloads integer
);

CREATE TABLE midas_resourcelog(
    id integer PRIMARY KEY AUTOINCREMENT,
    resource_id integer,
    resource_id_type integer,
    creation_date timestamp without time zone,
    lastmodified_date timestamp without time zone,
    size_bytes bigint,
    parentid integer
);

CREATE TABLE bitstream (
    bitstream_id integer NOT NULL,
    bitstream_format_id integer,
    name character varying(256),
    size_bytes bigint,
    checksum character varying(64),
    checksum_algorithm character varying(32),
    description text,
    user_format_description text,
    source character varying(256),
    internal_id character varying(256),
    deleted boolean,
    store_number integer,
    sequence_id integer
);

--
-- Name: bitstreamformatregistry; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE bitstreamformatregistry (
    bitstream_format_id integer NOT NULL,
    mimetype character varying(48),
    short_description character varying(128),
    description text,
    support_level integer,
    internal boolean
);

--
-- Name: bundle; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE bundle (
    bundle_id integer NOT NULL,
    mets_bitstream_id integer,
    name character varying(16),
    primary_bitstream_id integer
);

--
-- Name: bundle2bitstream; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE bundle2bitstream (
    id integer NOT NULL,
    bundle_id integer,
    bitstream_id integer
);

--
-- Name: checksum_history; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE checksum_history (
    check_id bigint NOT NULL,
    bitstream_id integer,
    process_start_date timestamp without time zone,
    process_end_date timestamp without time zone,
    checksum_expected character varying,
    checksum_calculated character varying,
    result character varying
);

--
-- Name: checksum_results; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE checksum_results (
    result_code character varying NOT NULL,
    result_description character varying
);

--
-- Name: collection; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE collection (
    collection_id integer NOT NULL,
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

--
-- Name: collection2item; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE collection2item (
    id integer NOT NULL,
    collection_id integer,
    item_id integer
);

--
-- Name: itemsbyauthor; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE itemsbyauthor (
    items_by_author_id integer NOT NULL,
    item_id integer,
    author text,
    sort_author text
);

--
-- Name: collectionitemsbyauthor; Type: VIEW; Schema: public; Owner: midas
--

CREATE VIEW collectionitemsbyauthor AS
    SELECT collection2item.collection_id, itemsbyauthor.items_by_author_id, itemsbyauthor.item_id, itemsbyauthor.author, itemsbyauthor.sort_author FROM itemsbyauthor, collection2item WHERE (itemsbyauthor.item_id = collection2item.item_id);

--
-- Name: itemsbydate; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE itemsbydate (
    items_by_date_id integer NOT NULL,
    item_id integer,
    date_issued text
);

--
-- Name: collectionitemsbydate; Type: VIEW; Schema: public; Owner: midas
--

CREATE VIEW collectionitemsbydate AS
    SELECT collection2item.collection_id, itemsbydate.items_by_date_id, itemsbydate.item_id, itemsbydate.date_issued FROM itemsbydate, collection2item WHERE (itemsbydate.item_id = collection2item.item_id);

--
-- Name: itemsbydateaccessioned; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE itemsbydateaccessioned (
    items_by_date_accessioned_id integer NOT NULL,
    item_id integer,
    date_accessioned text
);

--
-- Name: collectionitemsbydateaccession; Type: VIEW; Schema: public; Owner: midas
--

CREATE VIEW collectionitemsbydateaccession AS
    SELECT collection2item.collection_id, itemsbydateaccessioned.items_by_date_accessioned_id, itemsbydateaccessioned.item_id, itemsbydateaccessioned.date_accessioned FROM itemsbydateaccessioned, collection2item WHERE (itemsbydateaccessioned.item_id = collection2item.item_id);

--
-- Name: itemsbysubject; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE itemsbysubject (
    items_by_subject_id integer NOT NULL,
    item_id integer,
    subject text,
    sort_subject text
);

--
-- Name: collectionitemsbysubject; Type: VIEW; Schema: public; Owner: midas
--

CREATE VIEW collectionitemsbysubject AS
    SELECT collection2item.collection_id, itemsbysubject.items_by_subject_id, itemsbysubject.item_id, itemsbysubject.subject, itemsbysubject.sort_subject FROM itemsbysubject, collection2item WHERE (itemsbysubject.item_id = collection2item.item_id);

--
-- Name: itemsbytitle; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE itemsbytitle (
    items_by_title_id integer NOT NULL,
    item_id integer,
    title text,
    sort_title text
);

--
-- Name: collectionitemsbytitle; Type: VIEW; Schema: public; Owner: midas
--

CREATE VIEW collectionitemsbytitle AS
    SELECT collection2item.collection_id, itemsbytitle.items_by_title_id, itemsbytitle.item_id, itemsbytitle.title, itemsbytitle.sort_title FROM itemsbytitle, collection2item WHERE (itemsbytitle.item_id = collection2item.item_id);

--
-- Name: communities2item; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE communities2item (
    id integer NOT NULL,
    community_id integer,
    item_id integer
);

--
-- Name: community; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE community (
    community_id integer NOT NULL,
    name character varying(128),
    short_description character varying(512),
    introductory_text text,
    logo_bitstream_id integer,
    copyright_text text,
    side_bar_text text
);

--
-- Name: community2collection; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE community2collection (
    id integer NOT NULL,
    community_id integer,
    collection_id integer
);

--
-- Name: community2community; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE community2community (
    id integer NOT NULL,
    parent_comm_id integer,
    child_comm_id integer
);

--
-- Name: community2item; Type: VIEW; Schema: public; Owner: midas
--

CREATE VIEW community2item AS
    SELECT community2collection.community_id, collection2item.item_id FROM community2collection, collection2item WHERE (collection2item.collection_id = community2collection.collection_id);

--
-- Name: communityitemsbyauthor; Type: VIEW; Schema: public; Owner: midas
--

CREATE VIEW communityitemsbyauthor AS
    SELECT communities2item.community_id, itemsbyauthor.items_by_author_id, itemsbyauthor.item_id, itemsbyauthor.author, itemsbyauthor.sort_author FROM itemsbyauthor, communities2item WHERE (itemsbyauthor.item_id = communities2item.item_id);

--
-- Name: communityitemsbydate; Type: VIEW; Schema: public; Owner: midas
--

CREATE VIEW communityitemsbydate AS
    SELECT communities2item.community_id, itemsbydate.items_by_date_id, itemsbydate.item_id, itemsbydate.date_issued FROM itemsbydate, communities2item WHERE (itemsbydate.item_id = communities2item.item_id);

--
-- Name: communityitemsbydateaccession; Type: VIEW; Schema: public; Owner: midas
--

CREATE VIEW communityitemsbydateaccession AS
    SELECT communities2item.community_id, itemsbydateaccessioned.items_by_date_accessioned_id, itemsbydateaccessioned.item_id, itemsbydateaccessioned.date_accessioned FROM itemsbydateaccessioned, communities2item WHERE (itemsbydateaccessioned.item_id = communities2item.item_id);

--
-- Name: communityitemsbysubject; Type: VIEW; Schema: public; Owner: midas
--

CREATE VIEW communityitemsbysubject AS
    SELECT communities2item.community_id, itemsbysubject.items_by_subject_id, itemsbysubject.item_id, itemsbysubject.subject, itemsbysubject.sort_subject FROM itemsbysubject, communities2item WHERE (itemsbysubject.item_id = communities2item.item_id);

--
-- Name: communityitemsbytitle; Type: VIEW; Schema: public; Owner: midas
--

CREATE VIEW communityitemsbytitle AS
    SELECT communities2item.community_id, itemsbytitle.items_by_title_id, itemsbytitle.item_id, itemsbytitle.title, itemsbytitle.sort_title FROM itemsbytitle, communities2item WHERE (itemsbytitle.item_id = communities2item.item_id);

--
-- Name: metadatafieldregistry; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE metadatafieldregistry (
    metadata_field_id integer PRIMARY KEY AUTOINCREMENT,
    metadata_schema_id integer NOT NULL,
    element character varying(64),
    qualifier character varying(64),
    scope_note text
);

--
-- Name: metadatavalue; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE metadatavalue (
    metadata_value_id integer PRIMARY KEY AUTOINCREMENT,
    item_id integer,
    metadata_field_id integer,
    text_value text,
    text_lang character varying(24),
    place integer
);

--
-- Name: dcvalue; Type: VIEW; Schema: public; Owner: midas
--

CREATE VIEW dcvalue AS
    SELECT metadatavalue.metadata_value_id AS dc_value_id, metadatavalue.item_id, metadatavalue.metadata_field_id AS dc_type_id, metadatavalue.text_value, metadatavalue.text_lang, metadatavalue.place FROM metadatavalue, metadatafieldregistry WHERE ((metadatavalue.metadata_field_id = metadatafieldregistry.metadata_field_id) AND (metadatafieldregistry.metadata_schema_id = 1));

--
-- Name: eperson; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE eperson (
    eperson_id integer NOT NULL,
    email character varying(64),
    password character varying(64),
    firstname character varying(64),
    lastname character varying(64),
    can_log_in boolean,
    require_certificate boolean,
    self_registered boolean,
    last_active timestamp without time zone,
    sub_frequency integer,
    phone character varying(32),
    netid character varying(64),
    default_community integer DEFAULT 0
);

--
-- Name: epersongroup; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE epersongroup (
    eperson_group_id integer NOT NULL,
    name character varying(256)
);

--
-- Name: epersongroup2eperson; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE epersongroup2eperson (
    id integer NOT NULL,
    eperson_group_id integer,
    eperson_id integer
);

--
-- Name: epersongroup2workspaceitem; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE epersongroup2workspaceitem (
    id integer PRIMARY KEY AUTOINCREMENT,
    eperson_group_id integer,
    workspace_item_id integer
);

--
-- Name: fileextension; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE fileextension (
    file_extension_id integer NOT NULL,
    bitstream_format_id integer,
    extension character varying(16)
);

--
-- Name: group2group; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE group2group (
    id integer NOT NULL,
    parent_id integer,
    child_id integer
);

--
-- Name: group2groupcache; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE group2groupcache (
    id integer NOT NULL,
    parent_id integer,
    child_id integer
);

--
-- Name: handle; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE handle (
    handle_id integer NOT NULL,
    handle character varying(256),
    resource_type_id integer,
    resource_id integer
);

--
-- Name: history; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE history (
    history_id integer NOT NULL,
    creation_date timestamp without time zone,
    checksum character varying(32)
);

--
-- Name: historystate; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE historystate (
    history_state_id integer NOT NULL,
    object_id character varying(64)
);

--
-- Name: item; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE item (
    item_id integer NOT NULL,
    submitter_id integer,
    in_archive boolean,
    withdrawn boolean,
    last_modified timestamp with time zone,
    owning_collection integer
);

--
-- Name: item2bundle; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE item2bundle (
    id integer NOT NULL,
    item_id integer,
    bundle_id integer
);

--
-- Name: metadataschemaregistry; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE metadataschemaregistry (
    metadata_schema_id integer PRIMARY KEY AUTOINCREMENT,
    namespace character varying(256),
    short_id character varying(32)
);

--
-- Name: midas_bitstream; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE midas_bitstream (
    bitstream_id integer,
    date_uploaded character(255),
    downloads integer,
    revision integer
);

--
-- Name: midas_collectionsubmit; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE midas_collectionsubmit (
    collectionid integer,
    submission integer
);

--
-- Name: midas_searchsshfs; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE midas_searchsshfs (
    id integer,
    eperson_id integer,
    collection_id integer,
    expires timestamp without time zone
);

--
-- Name: midas_sshfs; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE midas_sshfs (
    id integer,
    type integer,
    links_directory character(255),
    communityid integer,
    hardlink integer
);

--
-- Name: midas_statistics; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE midas_statistics (
    id integer,
    type integer,
    ip character(28),
    datetime character(50),
    latitude character(10),
    longitude character(10)
);

--
-- Name: midas_userquota; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE midas_userquota (
    eperson_id integer,
    quota_mb integer
);

--
-- Name: most_recent_checksum; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE most_recent_checksum (
    bitstream_id integer NOT NULL,
    to_be_processed boolean NOT NULL,
    expected_checksum character varying NOT NULL,
    current_checksum character varying NOT NULL,
    last_process_start_date timestamp without time zone NOT NULL,
    last_process_end_date timestamp without time zone NOT NULL,
    checksum_algorithm character varying NOT NULL,
    matched_prev_checksum boolean NOT NULL,
    result character varying
);

--
-- Name: registrationdata; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE registrationdata (
    registrationdata_id integer NOT NULL,
    email character varying(64),
    token character varying(48),
    expires timestamp without time zone
);

--
-- Name: resourcepolicy; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE resourcepolicy (
    policy_id integer NOT NULL,
    resource_type_id integer,
    resource_id integer,
    action_id integer,
    eperson_id integer,
    epersongroup_id integer,
    start_date date,
    end_date date
);

--
-- Name: subscription; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE subscription (
    subscription_id integer NOT NULL,
    eperson_id integer,
    collection_id integer
);

--
-- Name: tasklistitem; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE tasklistitem (
    tasklist_id integer NOT NULL,
    eperson_id integer,
    workflow_id integer
);

--
-- Name: workflowitem; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE workflowitem (
    workflow_id integer NOT NULL,
    item_id integer,
    collection_id integer,
    state integer,
    owner integer,
    multiple_titles boolean,
    published_before boolean,
    multiple_files boolean
);


--
-- Name: workspaceitem; Type: TABLE; Schema: public; Owner: midas; Tablespace:
--

CREATE TABLE workspaceitem (
    workspace_item_id integer NOT NULL,
    item_id integer,
    collection_id integer,
    multiple_titles boolean,
    published_before boolean,
    multiple_files boolean,
    stage_reached integer
);

--- Create the midas_epersonapi table ---
CREATE TABLE midas_epersonapi (
    epersonapi_id integer PRIMARY KEY AUTOINCREMENT,
    eperson_id integer,
    apikey character varying(40),
    application_name character varying(256),
    token_expiration_time integer,
    creation_date date
);

CREATE INDEX epersonapi_epersonapi_id_idx ON midas_epersonapi (epersonapi_id);
CREATE INDEX epersonapi_eperson_id_idx ON midas_epersonapi (eperson_id);
CREATE INDEX epersonapi_application_name_id_idx ON midas_epersonapi (application_name);

--- Create the apitoken table ---
CREATE TABLE midas_apitoken (
    epersonapi_id integer,
    token character varying(40),
    expiration_date timestamp without time zone
);

CREATE INDEX apitoken_epersonapi_id_idx ON midas_apitoken (epersonapi_id);
CREATE INDEX apitoken_token_id_idx ON midas_apitoken (token);
CREATE INDEX apitoken_expiration_date_id_idx ON midas_apitoken (expiration_date);

INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (1, 'application/octet-stream', 'Unknown', 'Unknown data format', 0, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (2, 'text/plain', 'License', 'Item-specific license agreed upon to submission', 1, 'true');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (3, 'application/pdf', 'Adobe PDF', 'Adobe Portable Document Format', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (4, 'text/xml', 'XML', 'Extensible Markup Language', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (5, 'text/plain', 'Text', 'Plain Text', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (6, 'text/html', 'HTML', 'Hypertext Markup Language', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (7, 'text/css', 'CSS', 'Cascading Style Sheets', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (8, 'application/msword', 'Microsoft Word', 'Microsoft Word', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (9, 'application/vnd.ms-powerpoint', 'Microsoft Powerpoint', 'Microsoft Powerpoint', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (10, 'application/vnd.ms-excel', 'Microsoft Excel', 'Microsoft Excel', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (11, 'application/marc', 'MARC', 'Machine-Readable Cataloging records', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (12, 'image/jpeg', 'JPEG', 'Joint Photographic Experts Group/JPEG File Interchange Format (JFIF)', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (13, 'image/gif', 'GIF', 'Graphics Interchange Format', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (14, 'image/png', 'image/png', 'Portable Network Graphics', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (15, 'image/tiff', 'TIFF', 'Tag Image File Format', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (16, 'audio/x-aiff', 'AIFF', 'Audio Interchange File Format', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (17, 'audio/basic', 'audio/basic', 'Basic Audio', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (18, 'audio/x-wav', 'WAV', 'Broadcase Wave Format', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (19, 'video/mpeg', 'MPEG', 'Moving Picture Experts Group', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (20, 'text/richtext', 'RTF', 'Rich Text Format', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (21, 'application/vnd.visio', 'Microsoft Visio', 'Microsoft Visio', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (22, 'application/x-filemaker', 'FMP3', 'Filemaker Pro', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (23, 'image/x-ms-bmp', 'BMP', 'Microsoft Windows bitmap', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (24, 'application/x-photoshop', 'Photoshop', 'Photoshop', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (25, 'application/postscript', 'Postscript', 'Postscript Files', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (26, 'video/quicktime', 'Video Quicktime', 'Video Quicktime', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (27, 'audio/x-mpeg', 'MPEG Audio', 'MPEG Audio', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (28, 'application/vnd.ms-project', 'Microsoft Project', 'Microsoft Project', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (29, 'application/mathematica', 'Mathematica', 'Mathematica Notebook', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (30, 'application/x-latex', 'LateX', 'LaTeX document', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (31, 'application/x-tex', 'TeX', 'Tex/LateX document', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (32, 'application/x-dvi', 'TeX dvi', 'TeX dvi format', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (33, 'application/sgml', 'SGML', 'SGML application (RFC 1874)', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (34, 'application/wordperfect5.1', 'WordPerfect', 'WordPerfect 5.1 document', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (35, 'audio/x-pn-realaudio', 'RealAudio', 'RealAudio file', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (36, 'image/x-photo-cd', 'Photo CD', 'Kodak Photo CD image', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (37, 'image/metaimage', 'MetaImage', 'MetaImage File Format', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (38, 'dicom', 'DICOM', 'Dicom Image', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (39, 'archive zip', 'ZIP', 'Zip Archive', 1, 'false');
INSERT INTO bitstreamformatregistry (bitstream_format_id, mimetype, short_description, description, support_level, internal) VALUES (40, 'archive gzip', 'GZIP', 'GZip Archive', 1, 'false');

--
-- Data for Name: eperson; Type: TABLE DATA; Schema: public; Owner: midas
--

INSERT INTO eperson VALUES (1, 'admin@localhost', '200ceb26807d6bf99fd6f4f0d1ca54d4', 'Administrator', 'MIDAS', 'true', 'false', 'false', NULL, NULL, '00000', NULL, 0);

--
-- Data for Name: epersongroup; Type: TABLE DATA; Schema: public; Owner: midas
--

INSERT INTO epersongroup VALUES (0, 'Anonymous');
INSERT INTO epersongroup VALUES (1, 'Administrator');


--
-- Data for Name: epersongroup2eperson; Type: TABLE DATA; Schema: public; Owner: midas
--

INSERT INTO epersongroup2eperson VALUES (1, 1, 1);

--
-- Data for Name: fileextension; Type: TABLE DATA; Schema: public; Owner: midas
--

INSERT INTO fileextension VALUES (1, 3, 'pdf');
INSERT INTO fileextension VALUES (2, 4, 'xml');
INSERT INTO fileextension VALUES (3, 5, 'txt');
INSERT INTO fileextension VALUES (4, 5, 'asc');
INSERT INTO fileextension VALUES (5, 6, 'htm');
INSERT INTO fileextension VALUES (6, 6, 'html');
INSERT INTO fileextension VALUES (7, 7, 'css');
INSERT INTO fileextension VALUES (8, 8, 'doc');
INSERT INTO fileextension VALUES (9, 9, 'ppt');
INSERT INTO fileextension VALUES (10, 10, 'xls');
INSERT INTO fileextension VALUES (11, 12, 'jpeg');
INSERT INTO fileextension VALUES (12, 12, 'jpg');
INSERT INTO fileextension VALUES (13, 13, 'gif');
INSERT INTO fileextension VALUES (14, 14, 'png');
INSERT INTO fileextension VALUES (15, 15, 'tiff');
INSERT INTO fileextension VALUES (16, 15, 'tif');
INSERT INTO fileextension VALUES (17, 16, 'aiff');
INSERT INTO fileextension VALUES (18, 16, 'aif');
INSERT INTO fileextension VALUES (19, 16, 'aifc');
INSERT INTO fileextension VALUES (20, 17, 'au');
INSERT INTO fileextension VALUES (21, 17, 'snd');
INSERT INTO fileextension VALUES (22, 18, 'wav');
INSERT INTO fileextension VALUES (23, 19, 'mpeg');
INSERT INTO fileextension VALUES (24, 19, 'mpg');
INSERT INTO fileextension VALUES (25, 19, 'mpe');
INSERT INTO fileextension VALUES (26, 20, 'rtf');
INSERT INTO fileextension VALUES (27, 21, 'vsd');
INSERT INTO fileextension VALUES (28, 22, 'fm');
INSERT INTO fileextension VALUES (29, 23, 'bmp');
INSERT INTO fileextension VALUES (30, 24, 'psd');
INSERT INTO fileextension VALUES (31, 24, 'pdd');
INSERT INTO fileextension VALUES (32, 25, 'ps');
INSERT INTO fileextension VALUES (33, 25, 'eps');
INSERT INTO fileextension VALUES (34, 25, 'ai');
INSERT INTO fileextension VALUES (35, 26, 'mov');
INSERT INTO fileextension VALUES (36, 26, 'qt');
INSERT INTO fileextension VALUES (37, 27, 'mpa');
INSERT INTO fileextension VALUES (38, 27, 'abs');
INSERT INTO fileextension VALUES (39, 27, 'mpega');
INSERT INTO fileextension VALUES (40, 28, 'mpp');
INSERT INTO fileextension VALUES (41, 28, 'mpx');
INSERT INTO fileextension VALUES (42, 28, 'mpd');
INSERT INTO fileextension VALUES (43, 29, 'ma');
INSERT INTO fileextension VALUES (44, 30, 'latex');
INSERT INTO fileextension VALUES (45, 31, 'tex');
INSERT INTO fileextension VALUES (46, 32, 'dvi');
INSERT INTO fileextension VALUES (47, 33, 'sgm');
INSERT INTO fileextension VALUES (48, 33, 'sgml');
INSERT INTO fileextension VALUES (49, 34, 'wpd');
INSERT INTO fileextension VALUES (50, 35, 'ra');
INSERT INTO fileextension VALUES (51, 35, 'ram');
INSERT INTO fileextension VALUES (52, 36, 'pcd');
INSERT INTO fileextension VALUES (57, 37, 'mha');
INSERT INTO fileextension VALUES (58, 37, 'mhd');
INSERT INTO fileextension VALUES (59, 38, 'dcm');
INSERT INTO fileextension VALUES (60, 39, 'zip');
INSERT INTO fileextension VALUES (61, 40, 'gzip');

--
-- Data for Name: metadatafieldregistry; Type: TABLE DATA; Schema: public; Owner: midas
--

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
--
-- Data for Name: metadataschemaregistry; Type: TABLE DATA; Schema: public; Owner: midas
--

INSERT INTO metadataschemaregistry VALUES (1, 'http://dublincore.org/documents/dcmi-terms/', 'dc');


--
-- Data for Name: midas_userquota; Type: TABLE DATA; Schema: public; Owner: midas
--

INSERT INTO midas_userquota VALUES (1, 10240);


--
-- Name: bundle2bitstream_bundle_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX bundle2bitstream_bundle_idx ON bundle2bitstream (bundle_id);

--
-- Name: collection2item_collection_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX collection2item_collection_idx ON collection2item (collection_id);

--
-- Name: collection2item_item_id_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX collection2item_item_id_idx ON collection2item (item_id);

--
-- Name: communities2item_item_id_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX communities2item_item_id_idx ON communities2item (item_id);

--
-- Name: community2collection_collection_id_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX community2collection_collection_id_idx ON community2collection (collection_id);

--
-- Name: community2collection_community_id_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX community2collection_community_id_idx ON community2collection (community_id);

--
-- Name: date_issued_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX date_issued_idx ON itemsbydate (date_issued);

--
-- Name: eperson_email_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX eperson_email_idx ON eperson (email);

--
-- Name: eperson_netid_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX eperson_netid_idx ON eperson (netid);

--
-- Name: epersongroup2eperson_group_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX epersongroup2eperson_group_idx ON epersongroup2eperson (eperson_group_id);

--
-- Name: handle_handle_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX handle_handle_idx ON handle (handle);

--
-- Name: handle_resource_id_and_type_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX handle_resource_id_and_type_idx ON handle (resource_id, resource_type_id);

--
-- Name: item2bundle_item_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX item2bundle_item_idx ON item2bundle (item_id);

--
-- Name: itemsbyauthor_item_id_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX itemsbyauthor_item_id_idx ON itemsbyauthor (item_id);

--
-- Name: itemsbydate_item_id_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX itemsbydate_item_id_idx ON itemsbydate (item_id);

--
-- Name: itemsbydateaccessioned_item_id_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX itemsbydateaccessioned_item_id_idx ON itemsbydateaccessioned (item_id);

--
-- Name: itemsbytitle_item_id_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX itemsbytitle_item_id_idx ON itemsbytitle (item_id);

--
-- Name: metadatafield_schema_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX metadatafield_schema_idx ON metadatafieldregistry (metadata_schema_id);

--
-- Name: metadatavalue_item_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX metadatavalue_item_idx ON metadatavalue (item_id);

--
-- Name: metadatavalue_item_idx2; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX metadatavalue_item_idx2 ON metadatavalue (item_id, metadata_field_id);


--
-- Name: resourcepolicy_type_id_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX resourcepolicy_type_id_idx ON resourcepolicy (resource_type_id, resource_id);


--
-- Name: sort_author_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX sort_author_idx ON itemsbyauthor (sort_author);


--
-- Name: sort_subject_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX sort_subject_idx ON itemsbysubject (sort_subject);


--
-- Name: sort_title_idx; Type: INDEX; Schema: public; Owner: midas; Tablespace:
--

CREATE INDEX sort_title_idx ON itemsbytitle (sort_title);

---  Create the table mde_resource -- 
CREATE TABLE mde_resource (
    mde_resource_id integer PRIMARY KEY AUTOINCREMENT,
    name character varying(100),
    description character varying(100),
    sequence_id integer not null,
    type integer
);

CREATE TABLE mde_image (
    mde_image_id integer PRIMARY KEY AUTOINCREMENT,
    mde_resource_id integer NOT NULL,
    numberofdim integer,
    spx double precision,
    spy double precision,
    spz double precision,
    orx double precision,
    ory double precision,
    orz double precision,
    dimx integer,
    dimy integer,
    dimz integer
);

--
-- Name: mde_medicalimage; Type: TABLE; Schema: public; Owner: midas; Tablespace: 
--

CREATE TABLE mde_medicalimage (
    mde_medicalimage_id integer PRIMARY KEY AUTOINCREMENT,
    mde_image_id integer NOT NULL,
    format character varying (256),
    modality_id integer NOT NULL,
    type character varying (256)
);

--
-- Name: mde_device; Type: TABLE; Schema: public; Owner: midas; Tablespace: 
--

CREATE TABLE mde_device (
    mde_device_id integer PRIMARY KEY AUTOINCREMENT,
    acquisitionid character varying (256),
    manufacturer character varying (256),
    pixelpaddingvalue character varying (256),
    stationname character varying (256),
    modelname character varying (256),
    serialnumber character varying (256),
    softwareversion character varying (256),
    institutionaldepartmentname character varying (256),
    spatialresolution character varying (256),
    datelastcalibration character varying (256),
    timelastcalibration character varying (256)
);

--
-- Name: mde_device2medicalimage; Type: TABLE; Schema: public; Owner: midas; Tablespace: 
--

CREATE TABLE mde_device2medicalimage (
    id integer PRIMARY KEY AUTOINCREMENT,
    deviceid integer NOT NULL,
    medicalimageid integer NOT NULL
);

--
-- Name: mde_institution; Type: TABLE; Schema: public; Owner: midas; Tablespace: 
--

CREATE TABLE mde_institution (
    mde_institution_id integer PRIMARY KEY AUTOINCREMENT,
    name character varying (256),
    address character varying (256),
    codesequence character varying (256)
);

--
-- Name: mde_institution2medicalimage; Type: TABLE; Schema: public; Owner: midas; Tablespace: 
--
CREATE TABLE mde_institution2medicalimage (
    id integer PRIMARY KEY AUTOINCREMENT,
    institutionid integer NOT NULL,
    medicalimageid integer NOT NULL
);   

--
-- Name: mde_project; Type: TABLE; Schema: public; Owner: midas; Tablespace: 
--
CREATE TABLE mde_project (
    mde_project_id integer PRIMARY KEY AUTOINCREMENT,
    projectid double precision,
    date character varying (256),
    description text
);

--
-- Name: mde_project2medicalimage; Type: TABLE; Schema: public; Owner: midas; Tablespace: 
--
CREATE TABLE mde_project2medicalimage (
    id integer PRIMARY KEY AUTOINCREMENT,
    projectid integer NOT NULL,
    medicalimageid integer NOT NULL
);   

--
-- Name: mde_color
--
CREATE TABLE mde_color (
    mde_image_id bigint,
    colorid smallint,
    weight smallint
);
CREATE INDEX mde_color_mdeimageid_idx ON mde_color (mde_image_id);
CREATE INDEX mde_color_weight_idx ON mde_color (weight);

--
-- Name: mde_document
--
CREATE TABLE mde_document (
    mde_resource_id bigint,
    nwords integer
);
CREATE INDEX mde_document_mderesourceid_idx ON mde_document (mde_resource_id);

--
-- Name: mde_keyword
--
CREATE TABLE mde_keyword (
    mde_resource_id bigint,
    value character varying(50),
    frequency integer,
    weight real
);
CREATE INDEX mde_keyword_mderesourceid_idx ON mde_keyword (mde_resource_id);
CREATE INDEX mde_keyword_weight_idx ON mde_keyword (weight);

--
-- Name: mde_picture
--
CREATE TABLE mde_picture (
    mde_image_id bigint,
    exposure double precision,
    focal_length double precision,
    size_x smallint,
    size_y smallint
);
CREATE INDEX mde_picture_mdeimageid_idx ON mde_picture (mde_image_id);

--
-- Name: mde_subject
--
CREATE TABLE mde_subject (
    mde_subject_id integer PRIMARY KEY AUTOINCREMENT,
    subjectid character varying (256),
    name character varying (256),
    sex character varying (256),
    birthday character varying (256),
    age character varying (256),
    weight character varying (256),
    race character varying (256),
    dob character varying (256)
);

--
-- Name: mde_subject2medicalimage; Type: TABLE; Schema: public; Owner: midas; Tablespace: 
--
CREATE TABLE mde_subject2medicalimage (
    id integer PRIMARY KEY AUTOINCREMENT,
    subjectid integer NOT NULL,
    medicalimageid integer NOT NULL
);

--- Create the table thumbnail --
CREATE TABLE thumbnail (
  thumbnail_id integer PRIMARY KEY AUTOINCREMENT,
  image bytea NOT NULL
);

CREATE INDEX thumbnail_thumbnailid_idx ON thumbnail (thumbnail_id);

---  Create the table mderesource2thumbnail --  
CREATE TABLE mderesource2thumbnail (
  mde_resource_id integer NOT NULL,
  thumbnail_id integer NOT NULL
);

CREATE INDEX mderesource2thumbnail_mderesourceid_idx ON mderesource2thumbnail (mde_resource_id);
CREATE INDEX mderesource2thumbnail_thumbnailid_idx ON mderesource2thumbnail (thumbnail_id);

---  Create the table bitstream2thumbnail --   
CREATE TABLE bitstream2thumbnail (
  bitstream_id integer NOT NULL,
  thumbnail_id integer NOT NULL
);

CREATE INDEX bitstream2thumbnail_bitstreamid_idx ON bitstream2thumbnail (bitstream_id);
CREATE INDEX bitstream2thumbnail_thumbnailid_idx ON bitstream2thumbnail (thumbnail_id);    
    
--
-- Name: bitstream2resource for convenience because it should be too complicated to 
--       add the field resourceid into bitstream
CREATE TABLE bitstream2resource (
    id integer PRIMARY KEY AUTOINCREMENT,
    bitstreamid integer NOT NULL,
    resourceid integer NOT NULL
);
CREATE INDEX bitstream2resource_bitstreamid_idx ON bitstream2resource (bitstreamid);
CREATE INDEX bitstream2resource_resourceid_idx ON bitstream2resource (resourceid);

CREATE TABLE workflow (
    workflow_id integer PRIMARY KEY AUTOINCREMENT,
    eperson_id integer,
    where_start integer,
    name character varying(256),
    message character varying(256),
    creation_date date,
    active smallint NOT NULL DEFAULT 0
);

CREATE TABLE workflow_history (
    workflow_history_id integer PRIMARY KEY AUTOINCREMENT,
    workflow_id integer,
    creation_date date,
    resource_type_id integer,
    resource_id integer,
    userid integer
);

CREATE TABLE workflow_history_step (
    id integer PRIMARY KEY AUTOINCREMENT,
    workflow_history_id integer,
    type integer,
    resource_id integer,
    step integer
);

CREATE TABLE workflow_step (
    step_id integer PRIMARY KEY AUTOINCREMENT,
    workflow_id integer,
    step_number integer,
    resource_created integer,
    name character varying(256),
    description text,
    parent text,
    bitstream_step integer NOT NULL DEFAULT 0
);

CREATE TABLE workflow_step2field(  
    field_id integer PRIMARY KEY AUTOINCREMENT,  
    step_id integer NOT NULL,
    place integer,
    reference_field_id integer,
    type_field integer,
    step_number integer,
    name character varying(256),
    description text,
    possible_values text,
    size character varying(256),
    custom text,
    required integer DEFAULT 0
);

CREATE TABLE workflow_dependency(
    resource_type integer NOT NULL,
    resource_id integer NOT NULL,
    field_id integer,
    condition text
);

CREATE TABLE workflow_field(
    reference_field_id integer PRIMARY KEY AUTOINCREMENT,
    resource_type integer,
    possible_type character varying(256),
    description text,
    default_name character varying(256),
    db_name character varying(256),
    required integer
);
   
 CREATE TABLE community2collection_main(
    id integer PRIMARY KEY AUTOINCREMENT,
    community_id integer,
    collection_id integer UNIQUE
);  

 CREATE TABLE community2community_main(
    id integer PRIMARY KEY AUTOINCREMENT,
    parent_comm_id   integer,
    child_comm_id integer UNIQUE
);  

 CREATE TABLE filecache(
    id integer PRIMARY KEY AUTOINCREMENT,
    file text,
    size bigint,
    date timestamp,
    number_download integer,
    rank integer,
    cache varchar(2)
 );
 
 CREATE TABLE filecache_statistics(
    id integer PRIMARY KEY AUTOINCREMENT,
    name text,
    value bigint
 );
 
 CREATE TABLE filecache_statistics_details(
    id integer PRIMARY KEY AUTOINCREMENT,
    file text,
    size bigint,
    date timestamp,
    cache varchar(2)    
 );
 

 CREATE TABLE item2item(
    id integer PRIMARY KEY AUTOINCREMENT,
    itemid1 integer,
    itemid2 integer 
 );
 