#=============================================================================
#
#  Copyright (c) Kitware, Inc.
#  All rights reserved.
#  See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.  See the above copyright notice for more information.
#
#=============================================================================
"""
Test smtk.attribute.Collection.copyAttribute() method

Uses copyAttributeTest.sbi in the SMTKTestData repo.
"""

import logging
import os
import sys
import uuid

try:
    import smtk
    from smtk import attribute
    from smtk import io
    from smtk import model
except ImportError:
    print()
    print('Not able to import smtk library. You might need to:')
    print('  - Use the PYTHONPATH variable to point to the smtk python lib')
    print()
    sys.exit(-1)


logging.basicConfig(level=logging.DEBUG)

MODEL_FILENAME = 'test2D.json'
INPUT_FILENAME = 'copyAttributeTest.sbi'
OUTPUT_FILENAME = 'copyAttributeOut.sbi'

if __name__ == '__main__':
    # First (and) only argument is the path to the smtk data directory
    if len(sys.argv) < 2:
        print()
        print('Test smtk.attribute.Collection.copyAttribute()')
        print('Usage: python %s path-to-SMTKTestData')
        print()
        sys.exit(-1)

    # To help with debugging, write some environment vars
    logging.debug('LD_LIBRARY_PATH = %s' % os.environ.get('LD_LIBRARY_PATH'))
    logging.debug('PYTHONPATH = %s' % os.environ.get('PYTHONPATH'))

    smtk_test_data = sys.argv[1]

    #
    # Load smtk model file
    #
    model_path = os.path.join(smtk_test_data, 'model',
                              '2d', 'smtk', MODEL_FILENAME)
    logging.info('Reading %s' % model_path)
    json_string = None
    with open(model_path, 'r') as f:
        json_string = f.read()
    if json_string is None:
        logging.error('Unable to load input file %s' % model_path)
        sys.exit(-3)
    model_manager = smtk.model.Manager.create()
    ok = smtk.io.LoadJSON.intoModelManager(json_string, model_manager)
    if not ok:
        logging.error("Unable to create model from contents of %s" %
                      model_path)
        sys.exit(-4)

    #
    # Load attribute file into collection
    #
    att_folder = os.path.join(
        smtk_test_data, 'attribute', 'attribute_collection')
    att_path = os.path.join(att_folder, INPUT_FILENAME)
    logging.info('Reading %s' % att_path)
    input_collection = smtk.attribute.Collection.create()
    input_collection.setRefModelManager(model_manager)

    reader = smtk.io.AttributeReader()
    logger = smtk.io.Logger()
    err = reader.read(input_collection, att_path, logger)
    if err:
        logging.error("Unable to load template file")
        logging.error(logger.convertToString())
        sys.exit(-2)

    err_count = 0

    # Add model associations, using known UUID values (see test2D.xref)
    att_list = input_collection.findAttributes('FirstConcrete')
    if not att_list:
        logging.error("Unable to find FirstConcrete attribute")
        sys.exit(-2)
    first_concrete = att_list[0]

    vertex09 = '80640078-ee31-4117-ac6b-59261a7991d4'
    vertex_id = uuid.UUID(vertex09)
    first_concrete.associateEntity(vertex_id)

    att_list = input_collection.findAttributes('SecondConcrete')
    if not att_list:
        logging.error("Unable to find SecondConcrete attribute")
        sys.exit(-2)
    second_concrete = att_list[0]

    vertex14 = 'f90b9b63-6e55-4ba3-b690-bfb9d7bc3be1'
    vertex_id = uuid.UUID(vertex14)
    second_concrete.associateEntity(vertex_id)

    vertex15 = '05772ac6-920a-484c-b6b2-fc87b305bc17'
    vertex_id = uuid.UUID(vertex15)
    second_concrete.associateEntity(vertex_id)

    # Also test model entity item
    item = second_concrete.find('ModelFace')
    model_entity_item = smtk.attribute.to_concrete(item)
    face26 = '0bbc9685-e506-4fe7-b52f-ae52888d744b'
    face_id = uuid.UUID(face26)
    entityref = smtk.model.EntityRef(model_manager, face_id)
    model_entity_item.setValue(0, entityref)

    #
    # Instantiate 2nd/test collection
    #
    test_collection = smtk.attribute.Collection.create()
    test_collection.setRefModelManager(model_manager)
    # Copy SecondConcrete attribute
    options = smtk.attribute.Item.AssignmentOptions.COPY_MODEL_ASSOCIATIONS
    test_collection.copyAttribute(second_concrete, True, int(options))
    expected_deftypes = [
        'SecondConcrete', 'AnotherAbstractBase', 'CommonBase',
        'FirstConcrete', 'PolyLinearFunction'
    ]
    for def_type in expected_deftypes:
        defn = test_collection.findDefinition(def_type)
        if defn is None:
            logging.error('Expected %s definition, found None' % def_type)
            err_count += 1

    expected_atttypes = ['FirstConcrete',
                         'SecondConcrete', 'PolyLinearFunction']
    for att_type in expected_atttypes:
        att_list = test_collection.findAttributes(att_type)
        if len(att_list) != 1:
            logging.error('Expected %s attribute, found %d' %
                          (att_type, len(att_list)))
            err_count += 1

    # Note there is ALOT more that could & should be verified here
    logging.debug('Writing collection')

    # Write data out FYI
    writer = smtk.io.AttributeWriter()
    err = writer.write(test_collection, OUTPUT_FILENAME, logger)
    if err:
        logging.error("Unable to write output file")
        sys.exit(-6)
    logging.info('Wrote %s' % OUTPUT_FILENAME)

    # Check error count
    if err_count > 0:
        sys.exit(err_count)

    sys.exit(0)
