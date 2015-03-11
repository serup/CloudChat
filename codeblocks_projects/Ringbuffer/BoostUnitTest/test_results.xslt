<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- Edited by XMLSpy® -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
       
<xsl:template match="/">
  <html>
  <body>
	  <h2><fixture><xsl:value-of select="//TestSuite/@name"/>:</fixture>Test Results</h2>
    <table border="1">
      <tr bgcolor="#9acd32">
        <th>Name</th>
        <th>Result</th>
      </tr>
      <xsl:for-each select="//TestSuite/TestCase">
      <tr>
<!--        <td><fixture><xsl:value-of select="../@name"/>:</fixture><name><xsl:value-of select="@name" /></name></td> -->
        <td><name><xsl:value-of select="@name" /></name></td>
        <xsl:if test="@result = 'passed'" ><td bgcolor="#9acd32">success</td></xsl:if><xsl:if test="@result != 'passed'"><td bgcolor="#ff0000">failure</td></xsl:if>
      </tr>
      </xsl:for-each>
    </table>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>

