<?xml version="1.0" encoding="utf-8" ?>
<!-- Description of the polygon "SplitEdge" operator -->
<SMTK_AttributeResource Version="3">
  <Definitions>
    <!-- Operation -->
    <include href="smtk/operation/Operation.xml"/>
    <AttDef Type="split edge" Label="Edge - Split" BaseType="operation">
      <BriefDescription>Split a model edge at the given point.</BriefDescription>
      <DetailedDescription>
        Split a model edge in two at the given point.

        The given point must be a non-model-vertex point of the model edge.
        If the model edge has no model vertices, the result will be a single
        new edge with the given point promoted to a model vertex.
        Otherwise 2 new edges are created.
        Regardless, the input edge is always destroyed; it is never modified.
      </DetailedDescription>
      <AssociationsDef Name="edge" NumberOfRequiredValues="1" AdvanceLevel="1">
        <Accepts><Resource Name="smtk::session::polygon::Resource" Filter="edge"/></Accepts>
        <BriefDescription>The edge to split.</BriefDescription>
        <DetailedDescription>
          This is a model edge containing at least one point that is not a model-vertex.
          The edge will be removed and replaced by one or two new edges whose endpoint(s)
          are the model vertex.

          When the input edge is a loop with no model vertices,
          then the result is a new edge that has the model vertex as both endpoints;
          otherwise, 2 new model edges will be created.
        </DetailedDescription>
      </AssociationsDef>
      <ItemDefinitions>
        <Double Name="point" NumberOfRequiredValues="2" Extensible="yes" AdvanceLevel="1">
          <BriefDescription>The point where the edge should be split.</BriefDescription>
          <DetailedDescription>
            The world coordinates of the point where the edge should be split.
          </DetailedDescription>
        </Double>
        <Int Name="point id" NumberOfRequiredValues="1" AdvanceLevel="11">
          <DefaultValue>-1</DefaultValue>
          <BriefDescription>The ID of the point where the edge should be split.</BriefDescription>
          <DetailedDescription>
            The index of the point (in its tessellation) where the edge should be split.
          </DetailedDescription>
        </Int>
      </ItemDefinitions>
    </AttDef>
    <!-- Result -->
    <include href="smtk/operation/Result.xml"/>
    <AttDef Type="result(split edge)" BaseType="result">
      <ItemDefinitions>
        <!-- The edge(s) created are reported in the base result's "created" item. -->
        <!-- The input edge is destroyed and reported in the base result's "expunged" item. -->
      </ItemDefinitions>
    </AttDef>
  </Definitions>

  <Views>
    <View Type="smtkPolygonEdgeView" Title="Split Polygon Edge" UseSelectionManager="true">
      <AttributeTypes>
        <Att Type="split edge" />
      </AttributeTypes>
    </View>
  </Views>

</SMTK_AttributeResource>
