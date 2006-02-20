#!/usr/bin/env python

"""This is a minimal example of a nepentis central collection server.

In the current directory it woll create a SQLite database and a subdirectory for collected malware.
To use it you need python 2.3 and PySQLlite installed.

  --Maximillian Dornseif
"""

__rcsid__ = "$Id$"

import socket, time, datetime, os, os.path, md5, sys
from optparse import OptionParser
import SocketServer, BaseHTTPServer
import xmlrpclib
from SimpleXMLRPCServer import SimpleXMLRPCDispatcher, SimpleXMLRPCRequestHandler


class MyXMLRPCDispatcher(SimpleXMLRPCDispatcher):
 def _marshaled_dispatch(self, data, dispatch_method = None):
        params, method = xmlrpclib.loads(data)
	print "%s()" % method
        # generate response
        try:
            if dispatch_method is not None:
                response = dispatch_method(method, params)
            else:
                response = self._dispatch(method, params)
            # wrap response in a singleton tuple
            response = (response,)
            response = xmlrpclib.dumps(response, methodresponse=1)
        except Fault, fault:
            response = xmlrpclib.dumps(fault)
            print "%s:%s" % (sys.exc_type, sys.exc_value)
        except:
            # report exception back to server
            response = xmlrpclib.dumps(
                xmlrpclib.Fault(1, "%s:%s" % (sys.exc_type, sys.exc_value))
                )
            print "%s:%s" % (sys.exc_type, sys.exc_value)
        return response

class MySimpleXMLRPCServer(SocketServer.TCPServer,
                         MyXMLRPCDispatcher):
    def __init__(self, addr, requestHandler=SimpleXMLRPCRequestHandler,
                 logRequests=1):
        self.logRequests = logRequests
        self.allow_reuse_address = True
        SimpleXMLRPCDispatcher.__init__(self)
        SocketServer.TCPServer.__init__(self, addr, requestHandler)


HAVE_SQLITE = False

def startup_sqlite():
  """returns a cursor to a sqlite database and creates the needed tables if they don't exist"""
  from pysqlite2 import dbapi2 as sqlite
  global HAVE_SQLITE

  HAVE_SQLITE = True

  con = sqlite.connect(options.dbfile)
  cur = con.cursor()

  # create tables on demand
  cur.execute("SELECT name FROM sqlite_master WHERE type='table' and name='malware'")
  if not cur.fetchone():
    # add 'auto_increment' for mysql
    cur.execute("""
    CREATE TABLE malware (
      id          INTEGER PRIMARY KEY,
      md5         varchar(255) NOT NULL default '',
      first_seen  timestamp NOT NULL,
      metadata    text)
    """)

  cur.execute("SELECT name FROM sqlite_master WHERE type='table' and name='downloads'")
  if not cur.fetchone():
    # add 'auto_increment' for mysql
    cur.execute("""
    CREATE TABLE downloads (
      id INTEGER PRIMARY KEY,
      sensor varchar(255) NOT NULL default '<anonymous>',
      timestamp datetime NOT NULL default '0000-00-00 00:00:00',
      url varchar(255) NOT NULL default '<unknown>',
      schema varchar(255) NOT NULL default '',
      source varchar(255) NOT NULL default '',
      file varchar(255) NOT NULL default '',
      md5 varchar(255) NOT NULL default '',
      metadata text)
    """)

  return con, cur

def startup_mysql():
  """returns a curser to a mysql database connection"""

  import MySQLdb
  host, user, passwd, database = options.mysqlparms.split(':')
  con = MySQLdb.connect(db=database, user=user, passwd=passwd, host=host)
  return con, con.cursor()


def q(s):
  """fix query quoting for sqlite"""
  return s.replace('%s', '?')


def xmlrpc2sqltime(dt):
  return dt.value.replace('T', '').replace('-', '').replace(':', '')


def parseurl(url):
  """split a url in (schema, source, filename)
  
  This has certain knowledge on the details how nepenthes works."""

  schema = url.split('://')[0]
  filename = url.split('/')[-1]
  source = url.split('://')[1].split('@')[-1].split('/')[0].split(':')[0]
  if schema in ['creceive']:
    filename = ''

  return (schema, source, filename)



# XML-RPC exposed functions

def init_session(user, sysid, timestamp, metadata):
  """"init_session(user, sysid, timestamp, metadata)
  
  Sets up a session and returns a session token
  
  User should be your user/application id assigned to you by the server maintainer.
  
  Sysid should be the id of the machine you run the cliend  on and can be choosen freely by you 
  but should be unique for every machine.
  
  timestamp should be a float value indicaten the seconds since unix epoch on the client system
  
  metadata is an dictionary with additional information. So  far the following kjkeys are defined:
    clientsoftware: name and version of the software connectiong to the  server
  
  The returnes session Id is a string ans should be submitted by subsequent requests and a message of the day."""

  # FIXME: do actual session handling
  print "init_session %r" % ((user, sysid, timestamp, metadata),)

  return (md5.new(str(time.time())+sysid+user).hexdigest(), "Ok - Server $Rev$ welcomes you!")


def offer_malware(session, malmd5, first_seen, metadata):
  """offer_malware(session, md5, first_seen, metadata)
  
  Tells the server of a pice of malvare available at the client.
  
  Session is the value obtained by init_session().
  
  md5 is the md5 hash as a hex encoded string for the pice of malware.
  
  first_seen is the date the malware wasa first observed by the sensor.
  
  metadata is (for now) an empty dictionary.

  This function is mainly for transfering the contents of logging directories to the server. 
  The server will answer with True or False. If the answer is True the server requestsz the 
  client to use send_malware() to upload the file."""

  global con, cur

  print "offer_malware %r" % ((session, malmd5, first_seen, metadata),)

  cur.execute(q("select * from malware where md5=%s;"), (malmd5.replace("'", ""),))
  row = cur.fetchone()
  print row
  if row == None:
    cur.execute(q("insert into malware (md5, first_seen, metadata) values (%s, %s, %s);"), (malmd5,
                xmlrpc2sqltime(first_seen), repr(metadata)))
    con.commit()
    cur.execute("select * from malware order by id desc limit 1")
    print "offer_malware()", cur.fetchone()
    return True
  else:
    return False


def send_malware(session, malmd5, data, metadata):
  """send_malware(session, md5, data, metadata)
  
  Transmits a pice of malware to the server.
  
  Session is the value obtained by init_session().
  
  md5 is the md5 hash as a hex encoded string for the pice of malware.
  
  data is the actual malware.
  
  metadata is (for now) an empty dictionary.
  """

  global con, cur

  print "send_malware %r" % ((session, malmd5, "...", metadata),)

  cur.execute(q("select first_seen from malware where md5=%s"), (malmd5, ))
  row = cur.fetchone()
  if row == None:
    raise ValueError, "unknown malware, do offer_malware() first"

  # remove wrapper object
  data = data.data

  mymd5 = md5.new(data).hexdigest()
  print mymd5, malmd5
  if mymd5 != malmd5:
    raise ValueError, "md5 sum mismatch"

  fd = open(os.path.join(options.malwaredir, mymd5), 'w')
  fd.write(data)
  fd.close()
  first_seen = time.mktime(time.strptime(str(row[0]), "%Y%m%d%H%M%S"))
  os.utime(os.path.join(options.malwaredir, mymd5), (first_seen, first_seen))

  # FIXME: merge metadata - do not overwrite
  cur.execute(q("update malware set metadata = %s where md5 = %s"), (repr(metadata), mymd5))
  con.commit()
  cur.execute(q("select * from malware where md5 = %s"), ((malmd5),))
  print "send_malware()", cur.fetchone()

  return "Ok"


def log_download_success(session, timestamp, url, malmd5, metadata):
  """log_download_success(timestamp, url, md5, metadata)
  Logs a sucessfull download of malware ('submission' in nepenthes) from an infected host.

  Session is the value obtained by init_session().

  timestamp is the point in time when the malware was downloaded.
  
  url is the url the malware was downloaded from.
  
  md5 is the md5 hash as a hex encoded string for the pice of malware.

  metadata is (for now) an empty dictionary.
  
  The return value is the same as the return value of offer_malware()
  """

  global con, cur

  print "log_download_success %r" % ((session, timestamp, url, malmd5, metadata),)

  sensor = '<anonymous>'

  cur.execute(q("select * from downloads where sensor=%s and timestamp=%s and md5=%s and url=%s"),
              (sensor, xmlrpc2sqltime(timestamp), malmd5, url))
  row = cur.fetchone()
  if row == None:
    # no dupe - insert it

    schema, source, filename = parseurl(url)
    cur.execute(q("insert into downloads (sensor, timestamp, url, md5, metadata, schema, source, file) values (%s, %s, %s, %s, %s, %s, %s, %s);"),
                 (sensor, xmlrpc2sqltime(timestamp), url, malmd5, repr(metadata), schema, source, filename))
    con.commit()

  # shck if upload is wanted
  return offer_malware(session, malmd5, timestamp, metadata)


def log_download_attempt(session, timestamp, url, metadata):
  """log_download_attempt(timestamp, url, metadata)
  Logs an attempted download of malware ('download' in nepenthes) from an infected host.

  Session is the value obtained by init_session().

  timestamp is the point in time when the malware was downloaded.

  url is the url the malware was tried to be downloaded from.

  metadata is (for now) an empty dictionary.
  """

  global con, cur

  print "log_download_attempt %r" % ((timestamp, url, metadata),)

  sensor = '<anonymous>'

  cur.execute(q("select * from downloads where sensor=%s and timestamp=%s and url=%s"),
              (sensor, xmlrpc2sqltime(timestamp), url))
  row = cur.fetchone()
  if row == None:
    # no dupe - insert it

    schema, source, filename = parseurl(url)
    cur.execute(q("insert into downloads (sensor, timestamp, url, metadata, schema, source, file) values (%s, %s, %s, %s, %s, %s, %s);"),
                 (sensor, xmlrpc2sqltime(timestamp), url, repr(metadata), schema, source, filename))
    con.commit()

  return "Ok"


def breakdown():
  "crash and burn"
  10 / 0


# 212 this is just a test

def test():
  "primitive self test"

  print "testing init_session()",
  print init_session('userid', 'sensorid', xmlrpclib.DateTime(time.time()),
                     {'clientsoftware': 'rpcxmlxfer-test $Rev$'})
  print "testing offer_malware()"
  ret = offer_malware('session', 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa',
                      xmlrpclib.DateTime(time.time()), {'foo': 'bar', 'foobar': 'barfoo'})
  print ret
  try:
    print "testing send_malware() with broken md5"
    ret = send_malware('session', 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa', xmlrpclib.Binary('dummy'), {})
  except ValueError, msg:
    assert str(msg) == "md5 sum mismatch"
  data = 'testdata'
  md5sum = md5.new(data).hexdigest()
  try:
    print "testing send_malware() without offer_malware()"
    ret = send_malware('session', md5sum, xmlrpclib.Binary(data), {})
  except ValueError, msg:
    raise
    assert str(msg) == "unknown malware, do offer_malware() first"
  print "testing offer_malware()"
  ret = offer_malware('session', md5sum, xmlrpclib.DateTime(time.time()), {})
  print "testing send_malware() with offer_malware()"
  ret = send_malware('session', md5sum, xmlrpclib.Binary(data), {})
  for url in ['blink://127.0.0.1:22768/sUzzEA==', 'creceive://127.0.0.1:3247',
  'csend://127.0.0.1:8605/4', 'ftp://1:1@127.0.0.1:28796/eraseme_51646.exe',
  'ftp://wh0re:gotfucked@127.0.0.1:612/ass.exe',
  'ftp://127.0.0.1:20894/updating.pif', 'ftp://127.0.0.1/updating.pif',
  'http://127.0.0.1:80/xxxxxxxxx', 'link://127.0.0.1:24203/vuIF9A==',
  'tftp://127.0.0.1/Internet.exe']:
    log_download_success('session', xmlrpclib.DateTime(time.time()), url,
                         'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa', {})
    log_download_attempt('session', xmlrpclib.DateTime(time.time()), url, {})

# main ()
usage = "usage: %prog [options]"
parser = OptionParser(usage=usage, version='$Rev$')
parser.add_option("-p", dest="port", type="int",
                  default="4711",
                  help="listen on PORT [default: %default]")
parser.add_option("-l", dest="listenaddress", default="0.0.0.0",
                  help="bind to ADDRESS [default: %default]",
                  metavar='ADDRESS')
parser.add_option("-m", dest="malwaredir", type="string",
                  default="./malware",
                  help="write malware to MALWAREDIR [default: %default]")
parser.add_option("--sqlitedb", dest="dbfile", type="string",
                  default="./malware.db",
                    help="use DBFILE as SQLite database [default: %default]")
parser.add_option("--mysql", dest="mysqlparms", type="string",
                  default="",
                  help="connect to mysql database instead of using SQLite. Use MYSQLPARAMS to connect. [example: host:user:passwd:database]")
parser.add_option("--test", dest="test",
                  action="store_true", default=False,
                  help="selftest")
parser.add_option("-v",
                  action="count", dest="verbose", default=1,
                  help="print (more) status messages to stdout")
parser.add_option("-q", dest="quiet",
                  action="store_true", default=False,
                  help="be quiet")
(options, args) = parser.parse_args()

# FIXME: verbosity

if options.quiet:
  options.verbose = 0

if options.verbose > 2:
  print "rpcxmlxfer-client $Rev$ started."
  print options

if not os.path.exists(options.malwaredir):
  os.makedirs(options.malwaredir)

if options.mysqlparms:
  con, cur = startup_mysql()
else:
  con, cur = startup_sqlite()

server = MySimpleXMLRPCServer((options.listenaddress, options.port))
server.register_function(init_session)
server.register_function(offer_malware)
server.register_function(send_malware)
server.register_function(log_download_success)
server.register_function(log_download_attempt)
server.register_function(breakdown)
server.register_introspection_functions()
server.register_multicall_functions()

if options.test:
  test()
else:
  server.serve_forever()

