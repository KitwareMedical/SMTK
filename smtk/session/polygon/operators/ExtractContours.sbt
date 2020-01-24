<?xml version="1.0" encoding="utf-8" ?>
<!-- Description of the smtk polygon Model "extract contours" Operation -->
<SMTK_AttributeResource Version="3">
  <Definitions>
    <!-- Operation -->
    <include href="smtk/operation/Operation.xml"/>
    <AttDef Type="extract contours" Label="Edge - Create from Contours" BaseType="operation">
      <AssociationsDef Name="auxiliary geometry" NumberOfRequiredValues="1" AdvanceLevel="0">
        <Accepts><Resource Name="smtk::session::polygon::Resource" Filter="aux_geom"/></Accepts>
        <BriefDescription>The image auxiliary geometry to which this op will operate on.</BriefDescription>
        <DetailedDescription>
          The image auxiliary geometry to which this op will operate on. This auxiliary geometry should have an "url" string property.
        </DetailedDescription>
      </AssociationsDef>
      <ItemDefinitions>
        <Double Name="points" NumberOfRequiredValues="6" Extensible="yes" AdvanceLevel="1">
          <BriefDescription>The (x,y,z) coordinates of the edges.</BriefDescription>
          <DetailedDescription>
            The world coordinates of 1 or more edges.
          </DetailedDescription>
        </Double>
        <Int Name="coordinates" NumberOfRequiredValues="1" AdvanceLevel="1">
          <DefaultValue>3</DefaultValue>
          <BriefDescription>The number of coordinates per vertex.</BriefDescription>
          <DetailedDescription>
            When specifying coordinates for more than 1 vertex,
            this dictates how values are passed.
            When set to 2, the third coordinate is assumed to be 0 for all points.
          </DetailedDescription>
          <RangeInfo>
            <Min Inclusive="true">2</Min>
            <Max Inclusive="true">3</Max>
          </RangeInfo>
        </Int>
        <Int Name="offsets" NumberOfRequiredValues="1" Extensible="true" AdvanceLevel="1">
          <DefaultValue>0</DefaultValue>
          <BriefDescription>Offsets into the list of "points" where each edge starts.</BriefDescription>
          <DetailedDescription>
            Offsets into the list of points where each edge starts.

            When "edge points" are specified, each offset value is multiplied by 3.
            Thus, where "points" are passed, one would specify
            offsets equal to "[0, 3, 5]" to indicate the first edge has 3 points,
            the second edge has 2 points, and a third edge exists at the end after these two.
          </DetailedDescription>
        </Int>

        <Double Name="image bounds" NumberOfRequiredValues="6" Optional="true" AdvanceLevel="1">
          <DefaultValue>0., -1., 0., -1., 0., -1.</DefaultValue>
          <BriefDescription>The bounds of the image where the contours are extracted from.</BriefDescription>
          <DetailedDescription>
            This vector specifies the bounds of the image where the contours are generated from.
            This operator will try to set the model's origin in 3D to the center of the bounds,
            if the model does not have any entity with tessellation yet. This will make matching point
            coordinates between vtk and pmodel storage more robust, for example when doing edge split.
          </DetailedDescription>
        </Double>

        <!-- This is needed for linking with a vtkSMTKOperation that is used as an smtk operator interface
        to vtk pipeline -->
        <Int Name="HelperGlobalID" Label="Unique global ID for a helper object" AdvanceLevel="11" NumberOfRequiredValues="1" Optional="true">
          <DefaultValue>0</DefaultValue>
        </Int>
      </ItemDefinitions>
    </AttDef>
    <!-- Result -->
    <include href="smtk/operation/Result.xml"/>
    <AttDef Type="result(extract contours)" BaseType="result"/>
  </Definitions>

  <Views>
    <View Type="smtkPolygonContourView" Title="Extract Contour Edges" FilterByCategory="false" UseSelectionManager="true">
      <AttributeTypes>
        <Att Type="extract contours" />
      </AttributeTypes>
    </View>
  </Views>

</SMTK_AttributeResource>
