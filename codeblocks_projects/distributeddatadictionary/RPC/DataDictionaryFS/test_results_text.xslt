<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">
<xsl:value-of select="//TestSuite/@name"/>: Test Results
<xsl:for-each select="//TestSuite/TestCase">
<xsl:if test="@result = 'passed'" > 
 OK:   </xsl:if>
<xsl:if test="@result != 'passed'"> 
 FAIL: </xsl:if>
<xsl:value-of select="@name" />
</xsl:for-each>
</xsl:template>
</xsl:stylesheet>

