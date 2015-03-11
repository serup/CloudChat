Here are descriptions in xml format about the E/R Database
Every Entity's attributes are described here

see E/R diagram in systemspecs for detailed info on names of each entity

Each entity element has detailed info about it, and that detailed info follows
following format:

    <DDEntry>
        <DDGuid></DDGuid>
        <PhysicalDataElementName></PhysicalDataElementName>
        <Location></Location>
        <Category></Category>
        <DataType></DataType>
        <MaxLength></MaxLength>
        <Description></Description>
        <characteristics></characteristics>
        <relationship></relationship>
        <Mandatory>YES</Mandatory>
        <accessrights></accessrights>   
    </DDEntry>


The PhysicalDataElementName is the actual name of the attribut used in the code

Detailed information about the streams connections are described in the systemspecs

    /// Helpful script to cut attributes out of DD files
    /// $cat DD_PROFILE_TOAST_ATTRIBUTES.xml | grep -e '<PhysicalDataElementName>' | sed -e 's/<PhysicalDataElementName>/\"/' | sed -e 's/<\/PhysicalDataElementName>/\",/'



*****************
[10:08:48] johnnyserup: :) Nu fik jeg endelig løst problemet med hvordan vi kan gemme vores attributter i toast - faktisk kunne jeg ikke sove og hele natten lå jeg og tænkte på det dokument som du skrev omkring profile -- det slog mig at indtil flere attributter er i en slags kategorier og dette kunne jeg bruge :
[10:09:22] johnnyserup: Nu har jeg lavet en ekstra DD fil som beskriver det som ligger inde i TOAST data området -- så nu er der følgende filer :
[10:10:38] johnnyserup: 1. DD_<entity>.xml; 2. DD_<entity>_TOAST.xml; 3. DD_<entity>_TOAST_ATTRIBUTES.xml
[10:10:53] johnnyserup: fil nummer 3 er her det smarte kommer :)
[10:12:03] johnnyserup: filen 3 overholder protokol beskrevet i fil nummer 2, men indholdet i data records følger DED protokol og attributterne i DED entries er dem som der står i fil nummer 3 :)
[10:13:23] johnnyserup: Det betyder at entry i fil 3 angives med almindelig navne, men med en vigtig detalje og det er <Category> xxxx </Category>
[10:14:13] johnnyserup: denne kategori er navnet på vores DED struktur -- forstå det på denne måde:
[10:14:44] johnnyserup: hvis der i fil 3 er flere DDEntries som har samme kategori, så havner de i samme DED :)
[10:14:48] johnnyserup: *****************

