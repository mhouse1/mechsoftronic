import crck
# The response type returned by ProtocolWrapper
class ProtocolStatus(object):
    START_MSG = 'START_MSG'
    IN_MSG = 'IN_MSG'
    MSG_OK = 'MSG_OK'
    ERROR = 'ERROR'

class ProtocolWrapper(object):
    """ Wraps or unwraps a byte-stuffing header/footer protocol. 
    
        First, create an object with the desired parameters.
        Then, to wrap a data block with a protocol, simply call
        wrap().
        To unwrap, the object is used as a state-machine that is
        fed a byte at a time by calling input(). After each byte
        a ProtocolStatus is returned:
         * ERROR: check .last_error for the message
         * MSG_OK: the received message is .last_message
         * START_MSG: a new message has just begun (header 
           received)
         * IN_MSG: a message is in progress, so keep feeding bytes
        
        Bytes are binary strings one character long. I.e. 'a'
        means 0x61, '\x8B' means -0x8B.
        Messages - the one passed to wrap() and the one saved 
        in .last_message, are strings.
    """
    def __init__(self,
            header=91,
            footer=93,
            dle=35,
            after_dle_func=lambda x: x,
            keep_header=False,
            keep_footer=False):
        """ header: 
                The byte value that starts a message
            footer: 
                The byte value that ends a message
            dle:
                DLE value (the DLE is prepended to any header,
                footer and DLE in the stream)
            after_dle_func:
                Sometimes the value after DLE undergoes some 
                transormation. Provide the function that does 
                so here (i.e. XOR with some known value)
            keep_header/keep_footer:
                Keep the header/footer as part of the returned
                message.
        """
        self.header = chr(header)
        self.footer = chr(footer)
        self.dle = chr(dle)
        self.after_dle_func = after_dle_func
        self.keep_header = keep_header
        self.keep_footer = keep_footer
        
        self.state = self.WAIT_HEADER
        self.last_message = ''
        self.message_buf = ''
        self.last_error = ''
    
        self.last_crcvalue = 0
        self.crcvalue = 740712820

    def after_dle_func(self, byte):
        return byte
    
    def wrap(self, message):
        """ Wrap a message with header, footer and DLE according 
            to the settings provided in the constructor.
        """
        wrapped = self.header
#         self.last_crcvalue = self.crcvalue
#         self.crcvalue = crck.crc32(message, len(message),self.last_crcvalue)
        for b in message:
            if b in (self.header, self.footer, self.dle):
                wrapped += (self.dle + self.after_dle_func(b))
            else:
                wrapped += b
        #print wrapped
        #crchex = '%x'%self.crcvalue #get hex representation without 0x or L
        wrapwcrc = wrapped #+ crchex
        return wrapped+ self.footer 
    def wrapcrc(self, message):
        """ Wrap a message string with header, footer and DLE according 
            to the settings provided in the constructor.
        """
        wrapped = self.header
        self.last_crcvalue = self.crcvalue
        self.crcvalue = crck.crc32(message, len(message),self.last_crcvalue)
        for b in message:
            if b in (self.header, self.footer, self.dle):
                wrapped += (self.dle + self.after_dle_func(b))
            else:
                wrapped += b
        #print wrapped
        crchex = '%x'%self.crcvalue #get hex representation without 0x or L
        wrapwcrc = wrapped + crchex
        return str(wrapwcrc)+ self.footer        
    def wrapfields(self, message):
        """ message is a list of fields
            eg. for my_fields_list = [field1,field2,field3]
                this function will return "[field1][field2][field3]"
        Wrap a message with header, footer and DLE according 
            to the settings provided in the constructor.
        """

        wrapped_fields = ""
        for field in message:
            wrapped = self.header
            for b in field:
                if b in (self.header, self.footer, self.dle):
                    wrapped += (self.dle + self.after_dle_func(b))
                else:
                    wrapped += b
            #print wrapped
            single_wrapped_field = wrapped+ self.footer #add footer
            wrapped_fields = wrapped_fields +single_wrapped_field
        return wrapped_fields
    def wrapfieldscrc(self, message):
        """ message is a list of fields, this function returns
            wrapped fields with crc appended to the front
            eg. for my_fields_list = [field1,field2,field3]
                this function will build wrapped_fields_crc_appended 
                    "[crc32][field1][field2][field3]"
                then a final completed message is built that includes
                header and footer and escaping.
                    for example: for a input of
                    message = ['hello','world','i','am','michael']
                    the returned value is
                    "[\[14524a2b\]\[hello\]\[world\]\[i\]\[am\]\[michael\]]"
        Wrap a message with header, footer and DLE according 
            to the settings provided in the constructor.
            
            crc value is generated on the wrapped version of the fields list
            for example: 
            for a transmitted message [#[8885F03D#]#[###[gcode###]###[G90###]#]]
            the crc value 8885F03D is the crc of [gcode][G90]
            
            when get_fields is called on [#[8885F03D#]#[###[gcode###]###[G90###]#]]
            after each character has been sent as input to protocol wrapper
            layer1_field1 = 8885F03D
            layer1_field2 = [gcode][G90]
            when get_field is called on layer1_field2:
            layer2_field1 = gcode
            layer2_field2 = G90
            
            where layer2_field1 is the classification and layer2_field2 is type
        
            the design is such that layer2 can have any number of fields desired
            for example
            layer2_field1 = classification
            layer2_field2 = type
            layer2_field3 = data
            
        """
        wrapped_fields = ""
        for field in message:
            wrapped = self.header
            for b in field:
                if b in (self.header, self.footer, self.dle):
                    wrapped += (self.dle + self.after_dle_func(b))
                else:
                    wrapped += b
            #print wrapped
            single_wrapped_field = wrapped+ self.footer #add footer
            wrapped_fields = wrapped_fields +single_wrapped_field
        self.last_crcvalue = self.crcvalue
        #print 'fields list = ', message
        #wrapped_fields = self.wrap(wrapped_fields)
        #print 'wrapped_fields = ',wrapped_fields #[gcode][M05]
        
        self.crcvalue = crck.crc32(wrapped_fields, len(wrapped_fields),self.last_crcvalue)
        crchex = '%x'%self.crcvalue #get hex representation without 0x or L
        tobetransmitted = [crchex.upper()]
        #print "tobetransmitted ",tobetransmitted #'D22EF06B'
        tobetransmitted.append(wrapped_fields) #add wrapped fields to crc
        #print 'tobetransmitted appended ',tobetransmitted#['D22EF06B', '[gcode][M02]'] this is a list of 2 strings
        layer1 = self.wrapfields(tobetransmitted) 
        #print 'layer1 ',layer1 #[8885F03D][#[gcode#]#[G90#]]
        layer2 = self.wrap(layer1)
        #print 'decouple3 ',layer2 #[#[8885F03D#]#[###[gcode###]###[G90###]#]]
        return layer2        

    # internal state 
    (WAIT_HEADER, IN_MSG, AFTER_DLE) = range(3)
    
    def input(self, new_byte):
        """ Call this method whenever a new byte is received. It 
            returns a ProtocolStatus (see documentation of class
            for info).
        """
        
        if self.state == self.WAIT_HEADER:
            if new_byte == self.header:
                if self.keep_header:
                    self.message_buf += new_byte

                self.state = self.IN_MSG                    
                return ProtocolStatus.START_MSG
            else:
                self.last_error = 'Expected header (0x%02X), got 0x%02X' % (
                    ord(self.header), ord(new_byte))
                return ProtocolStatus.ERROR
        elif self.state == self.IN_MSG:
            if new_byte == self.dle:
                self.state = self.AFTER_DLE
                return ProtocolStatus.IN_MSG
            elif new_byte == self.footer:
                if self.keep_footer:
                    self.message_buf += new_byte
                self.message_buf
                return self._finish_msg()
                #return completed
            else: # just a regular message byte
                self.message_buf += new_byte
                return ProtocolStatus.IN_MSG
        elif self.state == self.AFTER_DLE:            
            self.message_buf += self.after_dle_func(new_byte)
            self.state = self.IN_MSG
            return ProtocolStatus.IN_MSG
        else:
            raise AssertionError()
    def get_fields(self):
        '''this must be called before input(byte) is called again
            so the self.last_message does not get overwritten
        '''
        pw = ProtocolWrapper()
        raw_fields = self.last_message
        field_list = []
        for byte in raw_fields:
            if pw.input(byte) == ProtocolStatus.MSG_OK:
                field_list.append(pw.last_message)
        return field_list
        
            
    def _finish_msg(self):
        self.state = self.WAIT_HEADER
        self.last_message = self.message_buf
        self.after_footer_count = -1
        self.message_buf = ''
        return ProtocolStatus.MSG_OK



if __name__ == '__main__':
    '''example use
    '''
    pw = ProtocolWrapper()
#     pww = pw.wrap('[Hello][w wor][ld i ][am mich][ael]')
#     print pww #[hel\\lo world!]
    myList = ['hello','world','i','am','michael']#3966c523
    pww = pw.wrapfieldscrc(myList)
    print pww #[\[14524a2b\]\[hello\]\[world\]\[i\]\[am\]\[michael\]]
    #input string
    message = '[#[14524A2B#]#[hello#]#[world#]#[i#]#[am#]#[michael#]]'
    
    #build message
    for byte in message:
        pw.input(byte) #returns protocol status
    print pw.last_message #[14524a2b][hello][world][i][am][michael]
    print pw.get_fields() #['14524a2b', 'hello', 'world', 'i', 'am', 'michael']
    
    
#     '''for using with serial port 
#     '''
#     while True:
#         #check if there are any (how many) bytes in buffer
#         BytesInBuffer = Communications.active_serial.inWaiting()
#         print BytesInBuffer,' Bytes in buffer'
#         if BytesInBuffer:
#             #if there are BytesInBuffer then read one byte   
#             #read one byte 
#             data = Communications.active_serial.read(1)
#             #use byte as input
#             if pw.input(byte) == ProtocolStatus.MSG_OK:
#                 #get fields if footer received
#                 fields_list = pw.get_fields()
#                 print fields_list
#         

#     print pww.encode('hex') #5b68656c5c5c6c6f20776f726c64215d
#     
#     pww = pw.wrap('goodbye cruel world!')
#     print pww
    
# [\[14524a2b\]\[hello\]\[world\]\[i\]\[am\]\[michael\]]
# START_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# IN_MSG
# MSG_OK
# [14524a2b][hello][world][i][am][michael]
# ['14524a2b', 'hello', 'world', 'i', 'am', 'michael']
    



    
    


