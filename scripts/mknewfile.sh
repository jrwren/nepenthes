# $Id$
#!/bin/sh

if [ $# -lt 1 ]
then
	echo "Usage `basename $0` FILE_0 FILE_1 .."
fi

for FILE in $@
do
echo -n "" > $FILE
echo "/***************************************************************************" >> $FILE
echo " *                      Inglewood Private License" >> $FILE
echo " *                      =========================" >> $FILE
echo " * " >> $FILE
echo " * Redistribution and use in binary forms, with or without modification," >> $FILE
echo " * are permitted provided that the following conditions are met:" >> $FILE
echo " * 1. The name of the author may not be used to endorse or promote products" >> $FILE
echo " *    derived from this software without specific prior written permission." >> $FILE
echo " * 2. The binary may not be sold and/or given away for free." >> $FILE
echo " * 3. The licensee may only create binaries for his own usage, not for any" >> $FILE
echo " *    third parties." >> $FILE
echo " * " >> $FILE
echo " * Redistribution and use in source forms, with or without modification," >> $FILE
echo " * are not permitted." >> $FILE
echo " * " >> $FILE
echo " * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR" >> $FILE
echo " * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES" >> $FILE
echo " * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED." >> $FILE
echo " * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT," >> $FILE
echo " * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT" >> $FILE
echo " * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE," >> $FILE
echo " * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY" >> $FILE
echo " * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT" >> $FILE
echo " * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF" >> $FILE
echo " * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE." >> $FILE
echo " *" >> $FILE
echo " ***************************************************************************/" >> $FILE
echo "" >> $FILE
echo "/* \$Id\$ */" >> $FILE

done
