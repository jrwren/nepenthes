#!/usr/bin/env python

"""This is a very simple example of a  nepentis central collection client.

It expects a subdirectory named binaries to contain malware samples. 
It then will communicate thiers existance to a central server.

  --Maximillian Dornseif
  
example usage in crontab:
  */5 * * * * /opt/nepenthes/bin/rpcxmlxfer-client -q

"""

__rcsid__ = "$Id$"

import sys, os, os.path, time, xmlrpclib, md5, socket
from optparse import OptionParser
from cPickle import dump, load, UnpicklingError

global options

def starttask(description):
    global starttime, options
    if options.verbose > 1:
      print description, "...",
    sys.stdout.flush()
    starttime = time.time()

def endtask(description=''):
    global starttime, options
    end = time.time()
    if options.verbose > 2:
      print description, "(%.3f s)" % (end-starttime)
    elif options.verbose > 1:
      print description

def upload_malware():
  """scan the malware dir and uploade when the server is in need"""
  global options, cache, session

  malware = 0
  uploads = 0

  alreadyseen = cache.setdefault('alreadyseen', {})

  if options.verbose:
    print "offering malware to server"

  for fn in os.listdir(options.bindir):
      if fn in alreadyseen:
        continue
      malware += 1
      starttask("offering %r" % fn)
      first_seen = os.stat(os.path.join(options.bindir, fn)).st_mtime
      wantupload = server.offer_malware(session, fn, xmlrpclib.DateTime(first_seen), {'foo': 'bar'})
      if wantupload:
          endtask("server wants upload")
          uploads += 1
          starttask("uploading")
          meta = {'foo': 'bar'}
          server.send_malware(session, fn,
            xmlrpclib.Binary(open(os.path.join(options.bindir, fn)).read()), meta)
          endtask("done")
      else:
          endtask("ok")
      alreadyseen[fn] = True

  if options.verbose:
    print "done uploading malware (out of %d new binaries %d where uploaded)" % (malware, uploads)

def preparelogline(line, alreadysent):
  global successes, attempts

  parts = line.strip().split()
  timestamp, url = parts[:2]
  if len(parts) > 2:
    md5sum = parts[2]
    successes  +=  1
  else:
    md5sum = ''
    attempts += 1
  timestamp = timestamp.strip('[]')
  if 'T' in timestamp:
    # we have a sane ISO timestamp.
    timestamp = time.mktime(time.strptime(timestamp.replace("-",""), "%Y%m%dT%H:%M:%S"))
  else:
    # insane timestamp =:-o  [23082005 23:09:18] all is b0rked!
    timestamp = ' '.join(parts[:2]).strip('[]')
    timestamp = time.mktime(time.strptime(timestamp, "%d%m%Y %H:%M:%S"))
    if len(parts) > 3:
      md5sum = parts[2]
      url = parts[3]
    else:
      url = parts[2]

  metadata = {}
  return timestamp, url, md5sum, metadata

def upload_downloadlogs():
  global options, cache, session
  global successes, attempts

  attempts = 0
  successes = 0

  if options.verbose:
    print "logging downloads to server"

  if options.verbose > 1:
    print "reading %r" % options.submlogfile
  fd = open(options.submlogfile)
  alreadysent = cache.setdefault('alreadysentsucessfulldownloads', {})
  for line in fd:
    line = line.strip()
    hash = md5.new(line).digest()
    if hash in alreadysent: continue
    alreadysent[hash] = True
    starttask(line)
    timestamp, url, md5sum, metadata = preparelogline(line, alreadysent)
    ret = server.log_download_success(session, xmlrpclib.DateTime(timestamp), url, md5sum, {'foo': 'bar'})
    endtask(ret)

  if options.verbose > 1:
    print "reading %r" % options.downllogfile
  fd = open(options.downllogfile)
  alreadysent = cache.setdefault('alreadysentsucessfulldownloads', {})
  for line in fd:
    line = line.strip()
    hash = md5.new(line).digest()
    if hash in alreadysent: continue
    alreadysent[hash] = True
    starttask(line)
    timestamp, url, md5sum, metadata = preparelogline(line, alreadysent)
    ret = server.log_download_attempt(session, xmlrpclib.DateTime(timestamp), url, {'foo': 'bar'})
    endtask(ret)

  if options.verbose:
    print "done logging downloads (%d new attempts and %d new successes logged)" % (attempts, successes)


# main ()
usage = "usage: %prog [options]"
parser = OptionParser(usage=usage, version='$Rev$')
parser.add_option("-p", dest="port", type="int",
                  default="4711",
                  help="connect PORT [default: %default]")
parser.add_option("-s", dest="serveraddress", default="malware.23.nu",
                  help="connect to server at ADDRESS [default: %default]",
                  metavar='ADDRESS')
parser.add_option("-u", dest="userid", default='<anonymous>',
                  help="identify as user USERID at server [default: %default]")
parser.add_option("-i", dest="sensorid", default=socket.gethostname(),
                  help="identify this sensor as SENSORID [default: %default]")
parser.add_option("-b", dest="bindir", type="string",
                  default="/opt/nepenthes/var/binaries",
                  help="read malware from BINDIR [default: %default]")
parser.add_option("-S", dest="submlogfile", type="string",
                  default="/opt/nepenthes/var/log/logged_submissions",
                  help="read successfull downloads form SUBLOGFILE [default: %default]")
parser.add_option("-D", dest="downllogfile", type="string",
                  default="/opt/nepenthes/var/log/logged_downloads",
                  help="read download attempts form DOWNLLOGFILE [default: %default]")
parser.add_option("-c", dest="cachefile", type="string",
                  default="/tmp/rpcxmlxfer.cache",
                  help="put persistent cache into CACHEFILE [default: %default]")
parser.add_option("-v",
                  action="count", dest="verbose", default=1,
                  help="print (more) status messages to stdout")
parser.add_option("-q", dest="quiet",
                  action="store_true", default=False,
                  help="be quiet")
parser.add_option("--server-doc", dest="getserverdoc",
                  action="store_true", default=False,
                  help="get protocol documentation from server and exit")
(options, args) = parser.parse_args()

if options.quiet:
  options.verbose = 0

if options.verbose > 2:
  print "rpcxmlxfer-client $Rev$ started."
  print options


if options.verbose:
  print "connecting to server %s:%d" % (options.serveraddress, options.port)
if options.verbose > 3:
  verbose = 1
else:
  verbose = 0
server =  xmlrpclib.ServerProxy("http://%s:%d" % (options.serveraddress, options.port), verbose=verbose)

if options.getserverdoc:
  for method in server.system.listMethods():
    if method.startswith('system'):
      continue
    print "%s():\n" % method, server.system.methodHelp(method), "\n", "-" * 50
else:
  # first we try to get the cache
  try:
    cache = load(open(options.cachefile))
    if options.verbose > 2:
      print "read cache %r" % options.cachefile
  except IOError:
    cache = {}
  except UnpicklingError:
    print "warning: cache corruption - flushig cache"
    cache = {}

  starttask("getting session")
  session, motd = server.init_session(options.userid, options.sensorid,
                                      xmlrpclib.DateTime(time.time()),
                                      {'clientsoftware': 'rpcxmlxfer-client $Rev$'})
  endtask(session)
  if verbose:
    print "Server says: %r" % motd

  upload_malware()

  dump(cache, open(options.cachefile, 'w'), -1)
  if options.verbose > 2:
    print "saved cache %r" % options.cachefile

  upload_downloadlogs()

  dump(cache, open(options.cachefile, 'w'), -1)
  if options.verbose > 2:
    print "saved cache %r" % options.cachefile
