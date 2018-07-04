<?xml version="1.0" encoding="utf-8" ?>
<!-- Description of the VTK "Export" Operator -->
<SMTK_AttributeResource Version="3">
  <Definitions>
    <include href="smtk/operation/Operation.xml"/>
    <AttDef Type="export" Label="Model - Export Resource" BaseType="operation">
      <AssociationsDef Name="Model(s)" NumberOfRequiredValues="1" Extensible="true">
        <MembershipMask>model</MembershipMask>
      </AssociationsDef>
      <ItemDefinitions>
        <File Name="filename" NumberOfRequiredValues="1"
          ShouldExist="false"
          FileFilters="Exodus II Datasets (*.e *.exo *.ex2);;Label maps (*.vti);; NetCDF files (*.nc *.ncdf);;All files (*.*)">
        </File>
        <String Name="filetype" NumberOfRequiredValues="1"/>
        <Double Name="nose sphere" NumberOfRequiredValues="4"/>
        <Double Name="lower plane" NumberOfRequiredValues="6"/>
      </ItemDefinitions>
    </AttDef>
    <!-- Result -->
    <include href="smtk/operation/Result.xml"/>
    <AttDef Type="result(export)" BaseType="result">
    </AttDef>
  </Definitions>
</SMTK_AttributeResource>
