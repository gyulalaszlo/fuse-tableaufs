#pragma once

// This file contains the definitions for the SQL fragments used by
// the postgres connector

#define TFS_WG_MTIME \
  ", extract(epoch from coalesce(c.updated_at,'2000-01-01')) ctime "

#define TFS_WG_LIST_SITES  \
  "select c.name" TFS_WG_MTIME "from sites c where 1 = 1 "

#define TFS_WG_LIST_PROJECTS \
  "select c.name" TFS_WG_MTIME "from projects c inner join" \
  " sites p on (p.id = c.site_id) where p.name = $1"

#define TFS_WG_LIST_FILE( entity, ext ) \
  "select c.name || '." #ext "' || case when substring(data from 1 for 2) = " \
  "'PK' then 'x' else '' end filename " TFS_WG_MTIME ", content," \
  "(select sum(length(data)) from pg_largeobject where pg_largeobject.loid = " \
  "repository_data.content) size from " #entity " c inner join repository_data " \
  " on (repository_data.id = coalesce(repository_data_id,repository_extract_data_id))" \
  "inner join projects on (c.project_id = projects.id) inner join sites on " \
  "(sites.id = projects.site_id) inner join pg_largeobject on " \
  "(repository_data.content = pg_largeobject.loid) where pg_largeobject.pageno = 0" \
  " and sites.name = $1 and projects.name = $2 "

#define TFS_WG_LIST_WORKBOOKS TFS_WG_LIST_FILE( workbooks, twb )

#define TFS_WG_LIST_DATASOURCES TFS_WG_LIST_FILE( datasources, tds )

#define TFS_WG_NAMES_WITHOUT_SLASH(ext) \
  "replace(c.name,'/','_')||'." #ext "x', replace(c.name,'/','_')||'." #ext "' "


