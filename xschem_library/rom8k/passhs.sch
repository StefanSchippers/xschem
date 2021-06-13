v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {// these are the stimulus parameters
// and are *NOT* the spec minimum/maximum values
// spec minimum/maximum values for timing checks are
// in the "timing check' section of module 'lpddr'

integer tis = 200; // 2ns... to be adjusted to reflect real spec data
integer tih = 200;
integer tck=1000;
integer tds=100;
integer tdh=100;
integer tdqss;
integer twpre;
integer twpst;

initial begin
tdqss=tck*5/4;
twpre = tck/4;
twpst = tck/2;
end



integer i,j;
integer wpst;
integer wpre;
integer tmp_wpst;
integer ndata, tmp_ndata;
integer data[511:0];
integer lmask[15:0], umask[15:0];
reg write_bus_cycles=0;
reg [1:0] iDQS_PAD = 2'bzz;
reg [15:0] iDQ_PAD='hz;

task init;
begin
CKE_PAD=0;
BA_PAD='bz;
APAD='bz;
CEN_PAD=1;
WEN_PAD=1'bz;
RASN_PAD=1'bz;
CASN_PAD=1'bz;
DM_PAD='hz;
#tis;
CK_PAD=0;
CKN_PAD=1;
#(tck/2);
CK_PAD=0;
CKN_PAD=1;
#(tck/2-tis);
end endtask

task deselect;
begin
CKE_PAD=1;
BA_PAD='bz;
APAD='bz;
CEN_PAD=1;
WEN_PAD=1'bz;
RASN_PAD=1'bz;
CASN_PAD=1'bz;
#tis;
CK_PAD=1;
CKN_PAD=0;
#tih;
CEN_PAD=1'bz;
#(tck/2-tih);
CK_PAD=0;
CKN_PAD=1;
#(tck/2-tis);
end endtask

task preactive;
input [8:0] sect;
input [1:0] bank;
begin
CKE_PAD=1;
BA_PAD=bank;
APAD=\{4'h0,sect\};
CEN_PAD=0;
WEN_PAD=0;
RASN_PAD=0;
CASN_PAD=1;
#tis;
CK_PAD=1;
CKN_PAD=0;
#tih;
APAD='bz;
BA_PAD='bz;
CEN_PAD=1'bz;
WEN_PAD=1'bz;
RASN_PAD=1'bz;
CASN_PAD=1'bz;
#(tck/2-tih);
CK_PAD=0;
CKN_PAD=1;
#(tck/2-tis);
end endtask


task active;
input [25:0] add;
input [1:0] bank;
begin
CKE_PAD=1;
BA_PAD=bank;
APAD=add[16:4];
CEN_PAD=0;
WEN_PAD=1;
RASN_PAD=0;
CASN_PAD=1;
#tis;
CK_PAD=1;
CKN_PAD=0;
#tih;
APAD='bz;
BA_PAD='bz;
CEN_PAD=1'bz;
WEN_PAD=1'bz;
RASN_PAD=1'bz;
CASN_PAD=1'bz;
#(tck/2-tih);
CK_PAD=0;
CKN_PAD=1;
#(tck/2-tis);
end endtask

task address;
input [25:0] add;
input [1:0] bank;
begin
CKE_PAD=1;
BA_PAD=bank;
APAD=\{4'h0,add[25:17]\};
CEN_PAD=0;
WEN_PAD=0;
RASN_PAD=0;
CASN_PAD=1;
#tis;
CK_PAD=1;
CKN_PAD=0;
#tih;
APAD='bz;
BA_PAD='bz;
CEN_PAD=1'bz;
WEN_PAD=1'bz;
RASN_PAD=1'bz;
CASN_PAD=1'bz;
#(tck/2-tih);
CK_PAD=0;
CKN_PAD=1;
#(tck/2-tis);


CKE_PAD=1;
BA_PAD=bank;
APAD=add[16:4];
CEN_PAD=0;
WEN_PAD=1;
RASN_PAD=0;
CASN_PAD=1;
#tis;
CK_PAD=1;
CKN_PAD=0;
#tih;
APAD='bz;
BA_PAD='bz;
CEN_PAD=1'bz;
WEN_PAD=1'bz;
RASN_PAD=1'bz;
CASN_PAD=1'bz;
#(tck/2-tih);
CK_PAD=0;
CKN_PAD=1;
#(tck/2-tis);

end endtask


task write;
input [12:0] add;
input [1:0] bank;
input preamble;
input postamble;
begin

  wpst = postamble;
  wpre = preamble;
  CKE_PAD=1;
  BA_PAD=bank;
  APAD=add;
  CEN_PAD=0;
  WEN_PAD=0;
  RASN_PAD=1;
  CASN_PAD=0;
  #tis;
  if(preamble) write_bus_cycles<= #(tdqss-twpre) 1;
  else write_bus_cycles<= #(tdqss-tds) 1;

  CK_PAD=1;
  CKN_PAD=0;
  #tih;
  APAD='bz;
  BA_PAD='bz;
  CEN_PAD=1'bz;
  WEN_PAD=1'bz;
  RASN_PAD=1'bz;
  CASN_PAD=1'bz;
  #(tck/2-tih);
  CK_PAD=0;
  CKN_PAD=1;
  #(tck/2-tis);
end endtask

always @(posedge write_bus_cycles)
begin
  tmp_ndata = ndata;
  tmp_wpst = wpst;
  $display("write_bus_cycles:ndata=%d", tmp_ndata);
  iDQS_PAD =0;
  if(wpre) begin
    #(twpre-tds);
  end
  for(i=0;i<tmp_ndata;i=i+1) begin
    iDQ_PAD = data[i];
    DM_PAD = 0;
    if(lmask[i]) DM_PAD[0] =1;
    if(umask[i]) DM_PAD[1] =1;
    #tds;
    if(iDQS_PAD==3) iDQS_PAD=0;
    else iDQS_PAD = 3;
    #tdh
    iDQ_PAD = 'hz;
    DM_PAD = 'hz;
    if(i<tmp_ndata-1) #((tck/2) - tds-tdh);
  end
  if(tmp_wpst) begin
    #(twpst-tdh);
    iDQS_PAD ='bzz;
  end

  write_bus_cycles=0;
end

assign DQS_PAD = iDQS_PAD;
assign DQ_PAD = iDQ_PAD;

task read;
input [12:0] add;
input [1:0] bank;
begin
CKE_PAD=1;
BA_PAD=bank;
APAD=add;
CEN_PAD=0;
WEN_PAD=1;
RASN_PAD=1;
CASN_PAD=0;
#tis;
CK_PAD=1;
CKN_PAD=0;
#tih;
APAD='bz;
BA_PAD='bz;
CEN_PAD=1'bz;
WEN_PAD=1'bz;
RASN_PAD=1'bz;
CASN_PAD=1'bz;
#(tck/2-tih);
CK_PAD=0;
CKN_PAD=1;
#(tck/2-tis);
end endtask

task nop;
begin
CKE_PAD=1;
CEN_PAD=0;
WEN_PAD=1;
RASN_PAD=1;
CASN_PAD=1;
#tis;
CK_PAD=1;
CKN_PAD=0;
#tih;
CEN_PAD=1'bz;
WEN_PAD=1'bz;
RASN_PAD=1'bz;
CASN_PAD=1'bz;
#(tck/2-tih);
CK_PAD=0;
CKN_PAD=1;
#(tck/2-tis);
end endtask

task rbt;
begin
CKE_PAD=1;
CEN_PAD=0;
WEN_PAD=0;
RASN_PAD=1;
CASN_PAD=1;
#tis;
CK_PAD=1;
CKN_PAD=0;
#tih;
CEN_PAD=1'bz;
WEN_PAD=1'bz;
RASN_PAD=1'bz;
CASN_PAD=1'bz;
#(tck/2-tih);
CK_PAD=0;
CKN_PAD=1;
#(tck/2-tis);
end endtask


task mrs;
input [12:0] add;
input [1:0] bank;
begin
CKE_PAD=1;
BA_PAD=bank;
APAD=add;
CEN_PAD=0;
WEN_PAD=0;
RASN_PAD=0;
CASN_PAD=0;
#tis;
CK_PAD=1;
CKN_PAD=0;
#tih;
APAD='bz;
BA_PAD='bz;
CEN_PAD=1'bz;
WEN_PAD=1'bz;
RASN_PAD=1'bz;
CASN_PAD=1'bz;

#(tck/2-tih);
CK_PAD=0;
CKN_PAD=1;
#(tck/2-tis);
end endtask

task open_pfow;
input [25:0] add;
begin
 
  mrs(\{6'b0_0010_0, add[25:19]\},3);     // MRS11 set PFOWBA(high 7 bits 25:19)
  nop;
  mrs(\{5'b0_0001, add[18:11]\},3);     // MRS11 set PFOWBA(low 8 bits 18:11)
  nop;
  mrs(13'b0_0000_0100_0000,3);     // MRS11 set PFOWBE
  nop;
end endtask 

task clear_sr;
input [25:0] pfow;
input [1:0] ba;
begin
  address(pfow+'h60,ba);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  // write command code, address
  lmask[ 0]=1; umask[ 0]=1; lmask[ 1]=1; umask[ 1]=1;
  lmask[ 2]=1; umask[ 2]=1; lmask[ 3]=1; umask[ 3]=1;
  lmask[ 4]=1; umask[ 4]=1; lmask[ 5]=1; umask[ 5]=1;
  lmask[ 6]=0; umask[ 6]=0; lmask[ 7]=1; umask[ 7]=1;
  lmask[ 8]=1; umask[ 8]=1; lmask[ 9]=1; umask[ 9]=1;
  lmask[10]=1; umask[10]=1; lmask[11]=1; umask[11]=1;
  lmask[12]=1; umask[12]=1; lmask[13]=1; umask[13]=1;
  lmask[14]=1; umask[14]=1; lmask[15]=1; umask[15]=1;

  data[ 0]='hffff; data[ 1]='hffff; data[ 2]='hffff; data[ 3]='hffff;
  data[ 4]='hffff; data[ 5]='hffff; data[ 6]='h0000; data[ 7]='hffff;
  data[ 8]='hffff; data[ 9]='hffff; data[10]='hffff; data[11]='hffff;
  data[12]='hffff; data[13]='hffff; data[14]='hffff; data[15]='hffff;
  ndata=16;
  write(0, ba,1,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
end endtask

task erase;
input [25:0] pfow;
input [25:0] add;
input [1:0] ba;
begin
  address(pfow+'h40,ba);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;

  // write command code, address
  lmask[ 0]=0; umask[ 0]=0; lmask[ 1]=1; umask[ 1]=1;
  lmask[ 2]=1; umask[ 2]=1; lmask[ 3]=1; umask[ 3]=1;
  lmask[ 4]=0; umask[ 4]=0; lmask[ 5]=0; umask[ 5]=0;
  lmask[ 6]=1; umask[ 6]=1; lmask[ 7]=1; umask[ 7]=1;
  lmask[ 8]=1; umask[ 8]=1; lmask[ 9]=1; umask[ 9]=1;
  lmask[10]=1; umask[10]=1; lmask[11]=1; umask[11]=1;
  lmask[12]=1; umask[12]=1; lmask[13]=1; umask[13]=1;
  lmask[14]=1; umask[14]=1; lmask[15]=1; umask[15]=1;

  data[ 0]='h0020; data[ 1]='hffff; data[ 2]='hffff; data[ 3]='hffff;
  data[ 4]=\{add[14:0],1'b0\}; data[ 5]=\{5'b0,add[25:15]\}; data[ 6]='hffff; data[ 7]='hffff;
  data[ 8]='h0200; data[ 9]='hffff; data[10]='hffff; data[11]='hffff;
  data[12]='hffff; data[13]='hffff; data[14]='hffff; data[15]='hffff;
  write(0, ba,1,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  address(pfow+'h60,ba);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  // write command execute
  lmask[ 0]=0; umask[ 0]=0; lmask[ 1]=1; umask[ 1]=1;
  lmask[ 2]=1; umask[ 2]=1; lmask[ 3]=1; umask[ 3]=1;
  lmask[ 4]=1; umask[ 4]=1; lmask[ 5]=1; umask[ 5]=1;
  lmask[ 6]=1; umask[ 6]=1; lmask[ 7]=1; umask[ 7]=1;
  lmask[ 8]=1; umask[ 8]=1; lmask[ 9]=1; umask[ 9]=1;
  lmask[10]=1; umask[10]=1; lmask[11]=1; umask[11]=1;
  lmask[12]=1; umask[12]=1; lmask[13]=1; umask[13]=1;
  lmask[14]=1; umask[14]=1; lmask[15]=1; umask[15]=1;

  data[ 0]='h0001; data[ 1]='hffff; data[ 2]='hffff; data[ 3]='hffff;
  data[ 4]='hffff; data[ 5]='hffff; data[ 6]='hffff; data[ 7]='hffff;
  data[ 8]='hffff; data[ 9]='hffff; data[10]='hffff; data[11]='hffff;
  data[12]='hffff; data[13]='hffff; data[14]='hffff; data[15]='hffff;
  write(0, ba,1,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
end endtask


// active and read from DAR's are performed on the given BA,
// write buffer filling is done using all 4 available BA's for
// pipelining write bursts
task write_buffer;
input [25:0] pfow;
input [25:0] add;
input [1:0] ba;
input npages;
integer npages;
integer i,j;
reg [1:0] iba;
begin
  ndata=16;
  address(pfow+'h60,ba);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  // clear write buffer
  lmask[ 0]=1; umask[ 0]=1; lmask[ 1]=1; umask[ 1]=1;
  lmask[ 2]=0; umask[ 2]=0; lmask[ 3]=1; umask[ 3]=1;
  lmask[ 4]=1; umask[ 4]=1; lmask[ 5]=1; umask[ 5]=1;
  lmask[ 6]=1; umask[ 6]=1; lmask[ 7]=1; umask[ 7]=1;
  lmask[ 8]=1; umask[ 8]=1; lmask[ 9]=1; umask[ 9]=1;
  lmask[10]=1; umask[10]=1; lmask[11]=1; umask[11]=1;
  lmask[12]=1; umask[12]=1; lmask[13]=1; umask[13]=1;
  lmask[14]=1; umask[14]=1; lmask[15]=1; umask[15]=1;

  data[ 0]='hffff; data[ 1]='hffff; data[ 2]='h0001; data[ 3]='hffff;
  data[ 4]='hffff; data[ 5]='hffff; data[ 6]='hffff; data[ 7]='hffff;
  data[ 8]='hffff; data[ 9]='hffff; data[10]='hffff; data[11]='hffff;
  data[12]='hffff; data[13]='hffff; data[14]='hffff; data[15]='hffff;
  write(0, ba,1,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;

  // fill write buffer ... uses all ba's for pipelining writes
  // pattern is data = address+ page number
  // note that the write buffer fill is really looking ugly, 
  // this is because maximum optimization is performed and
  // no single clock wait states occur during the 512 write cycles
  iba = ba;
  for(i=0;i<npages;i=i+1) begin

    // data array prepared
    // note that the first data prepared is not used 
    // so data prepared on loop i=0, j=0 is dummy
    // the first pattern really programmed is the one prepared on loop j=1
    for(j=0;j<16;j=j+1) begin  
      data[j] = i+j-1;  // data = address+page number; 
      umask[j]=0;       // the '-1' is because data writes occur later 
      lmask[j]=0;
    end
    // activate next page in next RDB
    preactive(pfow[25:17],iba);  
    active(pfow+'h400+i*16,iba); // data 0,1   already sent on DQ_PAD
    nop;			 // data 2,3   already sent on DQ_PAD
    nop;			 // data 4,5   already sent on DQ_PAD
    nop;			 // data 6,7   already sent on DQ_PAD
    nop;			 // data 8,9   already sent on DQ_PAD
    nop;			 // data 10,11 already sent on DQ_PAD
    nop;			 // data 12,13 already sent on DQ_PAD
    nop;			 // data 14,15 already sent on DQ_PAD
    nop;
    nop;
    nop;

       
				 // data 12,13 already sent on DQ_PAD

    write(0, iba,1,1);

    //if(i==0) write(0, iba,1,0);		// no postamble
    //else if(i==31) write(0, iba,0,1);   // no preamble
    //else write(0, iba,0,0);		// no preamble, no postamble
    iba = iba+1;		 	// increment RDB


  end
  // prepare data for last write going on in the background
  for(j=0;j<16;j=j+1) begin
    data[j] = i+j-1;
    umask[j]=0;
    lmask[j]=0;
  end
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  address(pfow+'h40,ba);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  // write command code, address, data count
  lmask[ 0]=0; umask[ 0]=0; lmask[ 1]=1; umask[ 1]=1;
  lmask[ 2]=0; umask[ 2]=0; lmask[ 3]=1; umask[ 3]=1;
  lmask[ 4]=0; umask[ 4]=0; lmask[ 5]=0; umask[ 5]=0;
  lmask[ 6]=1; umask[ 6]=1; lmask[ 7]=1; umask[ 7]=1;
  lmask[ 8]=0; umask[ 8]=0; lmask[ 9]=1; umask[ 9]=1;
  lmask[10]=1; umask[10]=1; lmask[11]=1; umask[11]=1;
  lmask[12]=1; umask[12]=1; lmask[13]=1; umask[13]=1;
  lmask[14]=1; umask[14]=1; lmask[15]=1; umask[15]=1;
  
  data[ 0]='h00e9; data[ 1]='hffff; data[ 2]='h0000; data[ 3]='hffff;
  data[ 4]=\{add[14:0],1'b0\}; data[ 5]=\{5'b0,add[25:15]\}; data[ 6]='hffff; data[ 7]='hffff;
  data[ 8]='h0200; data[ 9]='hffff; data[10]='hffff; data[11]='hffff;
  data[12]='hffff; data[13]='hffff; data[14]='hffff; data[15]='hffff;
  write(0, ba,1,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  address(pfow+'h60,ba);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  // write command execute
  lmask[ 0]=0; umask[ 0]=0; lmask[ 1]=1; umask[ 1]=1;
  lmask[ 2]=1; umask[ 2]=1; lmask[ 3]=1; umask[ 3]=1;
  lmask[ 4]=1; umask[ 4]=1; lmask[ 5]=1; umask[ 5]=1;
  lmask[ 6]=1; umask[ 6]=1; lmask[ 7]=1; umask[ 7]=1;
  lmask[ 8]=1; umask[ 8]=1; lmask[ 9]=1; umask[ 9]=1;
  lmask[10]=1; umask[10]=1; lmask[11]=1; umask[11]=1;
  lmask[12]=1; umask[12]=1; lmask[13]=1; umask[13]=1;
  lmask[14]=1; umask[14]=1; lmask[15]=1; umask[15]=1;
  
  data[ 0]='h0001; data[ 1]='hffff; data[ 2]='hffff; data[ 3]='hffff;
  data[ 4]='hffff; data[ 5]='hffff; data[ 6]='hffff; data[ 7]='hffff;
  data[ 8]='hffff; data[ 9]='hffff; data[10]='hffff; data[11]='hffff;
  data[12]='hffff; data[13]='hffff; data[14]='hffff; data[15]='hffff;
  write(0, ba,1,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
end endtask

// requires burst x 8 or x16 and start address page aligned
task poll_sr;
input [25:0] add;
input [1:0] ba;
begin
  address(add,ba);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  j=1;
  while(j) begin
    read('h0, ba);
    nop;
    nop;
    nop;
    nop;
    nop;
    if(DQ_PAD[7]==1) j=0;
    nop;
    nop;
  end
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
end endtask

task read_tests;
begin
  nop;
  preactive(23,3);
  active(26'h000aaa0,3);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  preactive(66,2);
  active(26'h0001230,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0000,3);
  nop;
  nop;
  nop;
  address('h1234560,0);
  nop;
  nop;
  read(13'h000b,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  active(26'h0002220,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0006,1);
  nop;
  active(26'h0005550,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0006,1);
  nop;
  nop;
  rbt;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0006,1);
  nop;
  active(26'h0001230,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  
  
  lmask[ 0]=0; umask[ 0]=0; lmask[ 1]=0; umask[ 1]=0;
  lmask[ 2]=0; umask[ 2]=0; lmask[ 3]=0; umask[ 3]=0;
  lmask[ 4]=0; umask[ 4]=0; lmask[ 5]=0; umask[ 5]=0;
  lmask[ 6]=0; umask[ 6]=0; lmask[ 7]=0; umask[ 7]=0;
  lmask[ 8]=0; umask[ 8]=0; lmask[ 9]=0; umask[ 9]=0;
  lmask[10]=0; umask[10]=0; lmask[11]=0; umask[11]=0;
  lmask[12]=0; umask[12]=0; lmask[13]=0; umask[13]=0;
  lmask[14]=0; umask[14]=0; lmask[15]=0; umask[15]=0;
  
  data[ 0]='h1111; data[ 1]='h2222; data[ 2]='h3333; data[ 3]='h4444;
  data[ 4]='h5555; data[ 5]='h6666; data[ 6]='h7777; data[ 7]='h8888;
  data[ 8]='h9999; data[ 9]='haaaa; data[10]='hbbbb; data[11]='hcccc;
  data[12]='hdddd; data[13]='heeee; data[14]='hffff; data[15]='h1111;
  ndata=16;
  write(13'h0001,2,1,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0000,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0005,3);
  nop;
  nop;
  rbt;
  nop;
  ndata=16;
  write(13'h0000,3,1,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0003,3);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0002,3);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0005,0);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  preactive(45,1);
  active(26'h0000880,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  ndata=6;
  write(13'h0000,1,1,1);
  nop;
  nop;
  read(13'h0002,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  active(26'h0001230,3);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  ndata=16;
  write(13'h0001,2,1,1);
  nop;
  nop;
  nop;
  active(26'h0004560,3);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0004,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;

  open_pfow('h2000000);
  nop;
  data[ 0]='h2111; data[ 1]='h3222; data[ 2]='h4333; data[ 3]='h5444;
  data[ 4]='h6555; data[ 5]='h7666; data[ 6]='h8777; data[ 7]='h9888;
  data[ 8]='ha999; data[ 9]='hbaaa; data[10]='hcbbb; data[11]='hdccc;
  data[12]='heddd; data[13]='hfeee; data[14]='h1fff; data[15]='h2111;
  address('h2000000,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0000,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  write(0,2,1,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  address('h2000000,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0000,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  address('h2000010,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0000,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  address('h2000020,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0000,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  address('h2000040,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0000,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  write(1,2,1,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  address('h2000040,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0000,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  address('h2000060,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0000,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  write(0,2,1,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  address('h2000060,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0000,2);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  preactive(0,2);
  nop;
  nop;
  nop;
  nop;
  mrs(13'b0_0000_0001_0010,1);     // MRS01 poll SRR18
  nop;
  read(13'b0_0000_0001_0010,1);
  mrs(13'b0_0000_0001_0011,1);     // MRS01 poll SRR19
  nop;
  read(13'b0_0000_0001_0011,1);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  while(xiddr.CMD_XCUTE) nop;
  address('h2000060,3);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  read(13'h0000,3);
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
end endtask

task program_tests;
begin

open_pfow(26'h2000000);
nop;
nop;
nop;
nop;
nop;
nop;
nop;
write_buffer(26'h2000000, 26'h300, 2'h0, 32);
nop;
nop;
nop;
poll_sr(26'h2000000+'h66,1);
nop;
nop;
nop;
nop;
address('h330,1);
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
read('h0, 1);
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
address('h330,0);
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
read('h0, 0);
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
address('h0,1);
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
read('h0, 1);
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
end endtask

initial begin
$dumpfile("dumpfile.vcd");
$dumpvars;
#2000;
init;
#2000;
deselect;
deselect;
deselect;
deselect;
mrs(13'b0_0000_1000_0000,3);     // MRS11 set DAI=1
nop;
i=0;
j=1;
while(j && i<200) begin
  i=i+1;
  mrs(13'b0_0000_0001_0010,1);     // MRS01 poll SRR18
  nop;
  read(13'b0_0000_0001_0010,1);
  nop;
  nop;
  if(DQ_PAD[7]==0) j=0;
  $display("checking DQ7: time=%t, DQ7=%d", $time, DQ_PAD[7]);
  nop;
end

nop;
mrs(13'h0004,0);  // set burst len to 16
nop;
nop;
nop;
nop;
nop;
nop;
program_tests;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;
nop;





$finish;
end

}
S {}
E {}
N 510 -350 510 -170 {lab=Z}
N 450 -350 450 -170 {lab=A}
N 480 -130 480 -110 {lab=E}
N 420 -110 480 -110 {lab=E}
N 420 -290 450 -290 {lab=A}
N 480 -410 480 -390 {lab=EN}
N 420 -410 480 -410 {lab=EN}
N 510 -290 540 -290 {lab=Z}
N 480 -350 480 -340 {lab=VDDPIN}
N 480 -180 480 -170 {lab=GNDPIN}
C {p.sym} 480 -370 3 1 {name=m60 model=cmosp w=WP l=2.4u m=1
}
C {n.sym} 480 -150 3 0 {name=m1 model=cmosn w=WN l=2.4u m=1}
C {iopin.sym} 540 -290 0 0 {name=p1 lab=Z}
C {iopin.sym} 420 -290 0 1 {name=p2 lab=A}
C {ipin.sym} 420 -410 0 0 {name=p3 lab=EN}
C {ipin.sym} 420 -110 0 0 {name=p4 lab=E}
C {lab_pin.sym} 480 -340 3 0 {name=l1 sig_type=std_logic lab=VCCPIN}
C {lab_pin.sym} 480 -180 3 1 {name=l2 sig_type=std_logic lab=VSSPIN}
C {title.sym} 160 0 0 0 {name=l3 author="Stefan Schippers"}
