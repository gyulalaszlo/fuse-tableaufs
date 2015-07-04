-- Table: repository_data

DROP TABLE repository_data;

CREATE TABLE repository_data
(
  id bigserial NOT NULL, -- Primary key for the record.
  name text, -- A possibly useful string that is mostly here to make it easier to debug.
  content oid, -- An oid field that references the corresponding data stored in the pg_largeobject table.
  created_at timestamp without time zone DEFAULT timezone('UTC'::text, now()), -- The time at which this record was created.
  CONSTRAINT pk_repository_data PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);
COMMENT ON TABLE repository_data
  IS 'This table forms the link between other tables that need to store "large object" data, and the large object storage, itself, which is in the pg_largeobject system table.';
COMMENT ON COLUMN repository_data.id IS 'Primary key for the record.';
COMMENT ON COLUMN repository_data.name IS 'A possibly useful string that is mostly here to make it easier to debug.';
COMMENT ON COLUMN repository_data.content IS 'An oid field that references the corresponding data stored in the pg_largeobject table.';
COMMENT ON COLUMN repository_data.created_at IS 'The time at which this record was created.';


-- Table: projects

DROP TABLE projects;

CREATE TABLE projects
(
  id serial NOT NULL, -- Primary key for the record.
  name character varying(255), -- The name of the project.
  owner_id integer NOT NULL, -- The id of the user who owns this project.
  created_at timestamp without time zone, -- The time at which this record was created.
  updated_at timestamp without time zone, -- The time at which this record was last changed.
  state character varying(32) DEFAULT 'active'::character varying, -- If this is anything other than "active" then the project is inactive.
  description text, -- A textual description of the project.
  site_id integer NOT NULL, -- The site which contains this project.
  special integer, -- Has a value of 1 if this is a special system-created project (like the default project). Otherwise it is NULL.
  luid uuid NOT NULL DEFAULT generate_uuid_safer(), -- The locally unique ID, which is intended for exposing externally by the server API. This is automatically generated.
  CONSTRAINT projects_pkey PRIMARY KEY (id),
  -- CONSTRAINT projects_owner_id_fkey FOREIGN KEY (owner_id)
      -- REFERENCES users (id) MATCH SIMPLE
      -- ON UPDATE CASCADE ON DELETE CASCADE DEFERRABLE INITIALLY IMMEDIATE,
  CONSTRAINT projects_site_id_fkey FOREIGN KEY (site_id)
      REFERENCES sites (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE DEFERRABLE INITIALLY IMMEDIATE,
  CONSTRAINT projects_luid_sites_id_unique UNIQUE (site_id, luid)
)
WITH (
  OIDS=FALSE
);
COMMENT ON TABLE projects
  IS 'Each row of the table corresponds to a project on Tableau Server.';
COMMENT ON COLUMN projects.id IS 'Primary key for the record.';
COMMENT ON COLUMN projects.name IS 'The name of the project.';
COMMENT ON COLUMN projects.owner_id IS 'The id of the user who owns this project.';
COMMENT ON COLUMN projects.created_at IS 'The time at which this record was created.';
COMMENT ON COLUMN projects.updated_at IS 'The time at which this record was last changed.';
COMMENT ON COLUMN projects.state IS 'If this is anything other than "active" then the project is inactive.';
COMMENT ON COLUMN projects.description IS 'A textual description of the project.';
COMMENT ON COLUMN projects.site_id IS 'The site which contains this project.';
COMMENT ON COLUMN projects.special IS 'Has a value of 1 if this is a special system-created project (like the default project). Otherwise it is NULL.';
COMMENT ON COLUMN projects.luid IS 'The locally unique ID, which is intended for exposing externally by the server API. This is automatically generated.';



-- Table: workbooks

-- DROP TABLE workbooks;

CREATE TABLE workbooks
(
  id serial NOT NULL, -- The primary key for the record.
  name character varying(255), -- The name of the workbook.
  repository_url text, -- A unique name for the workbook, derived from the ASCII characters in the name, which can be used in URLs to refer to this workbook.
  description text, -- SUPPRESS_DOC_OUTPUT: Description of workbook, but possibly not used.
  created_at timestamp without time zone, -- The time at which this record was created.
  updated_at timestamp without time zone, -- The last time any changes were made to this record.
  owner_id integer NOT NULL, -- A foreign key reference to the user that owns this workbook.
  project_id integer, -- A foreign key reference to the project in which this workbook exists.
  view_count integer, -- Keeps count of the number of views that are associated with this workbook.
  size bigint DEFAULT 0, -- The number of bytes used in storing the workbook information.
  embedded text DEFAULT ''::text, -- An encrypted keychain holding any credentials stored with the workbook.
  thumb_user integer, -- The user to be used when generating the thumbnail image. Null unless specified.
  refreshable_extracts boolean DEFAULT false, -- Whether the extracts in the workbook can be refreshed.
  extracts_refreshed_at timestamp without time zone, -- The time at which the last extract refresh occurred.
  lock_version integer DEFAULT 0, -- Used to implement "optimistic locking." Don't set this or mess with it, it's all handled automatically.
  state character varying(32) DEFAULT 'active'::character varying, -- SUPPRESS_DOC_OUTPUT: DocMe
  version character varying(255), -- SUPPRESS_DOC_OUTPUT: Looks like this isn't getting used.
  checksum character varying(255), -- SUPPRESS_DOC_OUTPUT: A checksum for the twbx. This checksum can be used to enforce the restriction that only the newest version of a workbook can be opened by the desktop. But devserver only has NULLs, so maybe it is Public only?
  display_tabs boolean DEFAULT false, -- If true, then the sheets of the workbook are displayed on tabs.
  data_engine_extracts boolean DEFAULT false, -- Indicates if this workbook has associated data engine extracts.
  incrementable_extracts boolean DEFAULT false, -- Indicates if it is possible to perform an incrementable extract refresh for this workbook.
  site_id integer NOT NULL, -- The site to which the workbook was published.
  revision character varying(255) DEFAULT '1.0'::character varying, -- Starts with 1.0 and increments by 0.1 with each republication.
  repository_data_id bigint, -- A reference to an entry in the repository_data table, which, in turn references the PostgreSQL large object that contains the workbook twbx contents. either this field or repository_extract_data_id should be non-NULL. Sometimes both could be non-NULL.
  repository_extract_data_id bigint, -- A reference to an entry in the repository_data table, which, in turn references the PostgreSQL large object that contains the workbook twbx contents minus any extract data. either this field or repository_data_id should be non-NULL. Sometimes both could be non-NULL.
  first_published_at timestamp without time zone, -- Records the timestamp for the first time this view was published. This value will not change is the workbook containing this view is later republished.
  primary_content_url character varying(255), -- SUPPRESS_DOC_OUTPUT: Not used?
  share_description character varying(255), -- SUPPRESS_DOC_OUTPUT
  show_toolbar boolean DEFAULT true, -- SUPPRESS_DOC_OUTPUT: Always seems to be true.  Is it used at all?  Maybe Public?
  extracts_incremented_at timestamp without time zone, -- The time of the last incremental extract refresh for extracts in this workbook.
  default_view_index integer, -- Indicates which view will be shown by default.
  luid uuid NOT NULL DEFAULT generate_uuid_safer(), -- An immutable identifier for the record.
  asset_key_id integer, -- The asset encryption key used to encrypt the keychain in the embedded column.
  document_version character varying(255), -- A string that indicates the version of the document description that was used when saving this workbook.
  CONSTRAINT workbooks_pkey PRIMARY KEY (id),
  -- CONSTRAINT workbooks_owner_id_fkey FOREIGN KEY (owner_id)
      -- REFERENCES users (id) MATCH SIMPLE
      -- ON UPDATE CASCADE ON DELETE CASCADE DEFERRABLE INITIALLY IMMEDIATE,
  CONSTRAINT workbooks_project_id_fkey FOREIGN KEY (project_id)
      REFERENCES projects (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE DEFERRABLE INITIALLY IMMEDIATE,
  CONSTRAINT workbooks_site_id_fkey FOREIGN KEY (site_id)
      REFERENCES sites (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE DEFERRABLE INITIALLY IMMEDIATE,
  CONSTRAINT workbooks_luid_sites_id_unique UNIQUE (site_id, luid)
)
WITH (
  OIDS=FALSE
);
COMMENT ON TABLE workbooks
  IS 'Each record represents a workbook that exists on the server.';
COMMENT ON COLUMN workbooks.id IS 'The primary key for the record.';
COMMENT ON COLUMN workbooks.name IS 'The name of the workbook.';
COMMENT ON COLUMN workbooks.repository_url IS 'A unique name for the workbook, derived from the ASCII characters in the name, which can be used in URLs to refer to this workbook.';
COMMENT ON COLUMN workbooks.description IS 'SUPPRESS_DOC_OUTPUT: Description of workbook, but possibly not used.';
COMMENT ON COLUMN workbooks.created_at IS 'The time at which this record was created.';
COMMENT ON COLUMN workbooks.updated_at IS 'The last time any changes were made to this record.';
COMMENT ON COLUMN workbooks.owner_id IS 'A foreign key reference to the user that owns this workbook.';
COMMENT ON COLUMN workbooks.project_id IS 'A foreign key reference to the project in which this workbook exists.';
COMMENT ON COLUMN workbooks.view_count IS 'Keeps count of the number of views that are associated with this workbook.';
COMMENT ON COLUMN workbooks.size IS 'The number of bytes used in storing the workbook information.';
COMMENT ON COLUMN workbooks.embedded IS 'An encrypted keychain holding any credentials stored with the workbook.';
COMMENT ON COLUMN workbooks.thumb_user IS 'The user to be used when generating the thumbnail image. Null unless specified.';
COMMENT ON COLUMN workbooks.refreshable_extracts IS 'Whether the extracts in the workbook can be refreshed.';
COMMENT ON COLUMN workbooks.extracts_refreshed_at IS 'The time at which the last extract refresh occurred.';
COMMENT ON COLUMN workbooks.lock_version IS 'Used to implement "optimistic locking." Don''t set this or mess with it, it''s all handled automatically.';
COMMENT ON COLUMN workbooks.state IS 'SUPPRESS_DOC_OUTPUT: DocMe';
COMMENT ON COLUMN workbooks.version IS 'SUPPRESS_DOC_OUTPUT: Looks like this isn''t getting used.';
COMMENT ON COLUMN workbooks.checksum IS 'SUPPRESS_DOC_OUTPUT: A checksum for the twbx. This checksum can be used to enforce the restriction that only the newest version of a workbook can be opened by the desktop. But devserver only has NULLs, so maybe it is Public only?';
COMMENT ON COLUMN workbooks.display_tabs IS 'If true, then the sheets of the workbook are displayed on tabs.';
COMMENT ON COLUMN workbooks.data_engine_extracts IS 'Indicates if this workbook has associated data engine extracts.';
COMMENT ON COLUMN workbooks.incrementable_extracts IS 'Indicates if it is possible to perform an incrementable extract refresh for this workbook.';
COMMENT ON COLUMN workbooks.site_id IS 'The site to which the workbook was published.';
COMMENT ON COLUMN workbooks.revision IS 'Starts with 1.0 and increments by 0.1 with each republication.';
COMMENT ON COLUMN workbooks.repository_data_id IS 'A reference to an entry in the repository_data table, which, in turn references the PostgreSQL large object that contains the workbook twbx contents. either this field or repository_extract_data_id should be non-NULL. Sometimes both could be non-NULL.';
COMMENT ON COLUMN workbooks.repository_extract_data_id IS 'A reference to an entry in the repository_data table, which, in turn references the PostgreSQL large object that contains the workbook twbx contents minus any extract data. either this field or repository_data_id should be non-NULL. Sometimes both could be non-NULL.';
COMMENT ON COLUMN workbooks.first_published_at IS 'Records the timestamp for the first time this view was published. This value will not change is the workbook containing this view is later republished.';
COMMENT ON COLUMN workbooks.primary_content_url IS 'SUPPRESS_DOC_OUTPUT: Not used?';
COMMENT ON COLUMN workbooks.share_description IS 'SUPPRESS_DOC_OUTPUT';
COMMENT ON COLUMN workbooks.show_toolbar IS 'SUPPRESS_DOC_OUTPUT: Always seems to be true.  Is it used at all?  Maybe Public?';
COMMENT ON COLUMN workbooks.extracts_incremented_at IS 'The time of the last incremental extract refresh for extracts in this workbook.';
COMMENT ON COLUMN workbooks.default_view_index IS 'Indicates which view will be shown by default.';
COMMENT ON COLUMN workbooks.luid IS 'An immutable identifier for the record.';
COMMENT ON COLUMN workbooks.asset_key_id IS 'The asset encryption key used to encrypt the keychain in the embedded column.';
COMMENT ON COLUMN workbooks.document_version IS 'A string that indicates the version of the document description that was used when saving this workbook.';
