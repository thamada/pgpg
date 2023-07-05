                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_fix_addsub is                                            
  generic(PG_WIDTH: integer;                                       
          PG_PIPELINE: integer;                                    
          PG_DIRECTION: string);                                   
  port(                                                            
       x,y : in std_logic_vector(PG_WIDTH-1 downto 0);             
       z : out std_logic_vector(PG_WIDTH-1 downto 0);              
       clk : in std_logic                                          
  );                                                               
end pg_fix_addsub;                                                 
                                                                   
architecture rtl of pg_fix_addsub is                               
                                                                   
 component lpm_add_sub                                             
  generic (LPM_WIDTH: integer;                                     
	   LPM_PIPELINE: integer;                                    
	   LPM_DIRECTION: string);                                   
  port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);    
      clock: in std_logic;                                         
      result: out std_logic_vector(LPM_WIDTH-1 downto 0));         
  end component;                                                   
                                                                   
begin                                                              
                                                                   
  u1: lpm_add_sub generic map (LPM_WIDTH=>PG_WIDTH,                
                LPM_PIPELINE=>PG_PIPELINE,LPM_DIRECTION=>PG_DIRECTION)
         port map(dataa=>x,datab=>y,clock=>clk,result=>z);         
                                                                   
end rtl;                                                           
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_conv_ftol_24_14_5 is         
  port(fixdata : in std_logic_vector(23 downto 0);      
       logdata : out std_logic_vector(13 downto 0);     
       clk : in std_logic);                                        
end pg_conv_ftol_24_14_5;              
                                                                   
architecture rtl of pg_conv_ftol_24_14_5 is 
                                                                   
  component table                                                  
  generic (IN_WIDTH: integer ;                                     
           OUT_WIDTH: integer ;                                    
           TABLE_FILE: string);                                    
  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          
      outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        
      clk : in std_logic);                                         
  end component;                                                   
                                                                   
  component lpm_add_sub                                            
    generic (LPM_WIDTH: integer;                                   
             LPM_DIRECTION: string);                               
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     
  end component;                                                   
                                                                   
  component penc_23_5                        
    port( a : in std_logic_vector(22 downto 0);        
          c : out std_logic_vector(4 downto 0));      
  end component;                                                   
                                                                   
  component shift_ftol_22_7                 
    port( indata : in std_logic_vector(21 downto 0);   
          control : in std_logic_vector(4 downto 0);  
          outdata : out std_logic_vector(6 downto 0));
  end component;                                                   
                                                                   
  signal d1,d0: std_logic_vector(22 downto 0);         
  signal d2: std_logic_vector(22 downto 0);            
  signal d3,d3r: std_logic_vector(22 downto 0);        
  signal one: std_logic_vector(22 downto 0);           
  signal sign: std_logic;                                          
  signal c1: std_logic_vector(4 downto 0);            
  signal d4: std_logic_vector(21 downto 0);            
  signal c2,c3,c4,add: std_logic_vector(4 downto 0);  
  signal d5,d6: std_logic_vector(6 downto 0);         
  signal sign0,sign0r,sign1,sign2,sign3: std_logic;                
  signal nz0,nz1,nz2: std_logic;                                   
                                                                   
begin                                                              
                                                                   
  d1 <=  NOT fixdata(22 downto 0);                     
  one <= "00000000000000000000001";                                              
  u1: lpm_add_sub generic map (LPM_WIDTH=>23,LPM_DIRECTION=>"ADD")
                  port map(result=>d2,dataa=>d1,datab=>one);     
  d0 <= fixdata(22 downto 0);                        
  sign0 <= fixdata(23);                              
                                                                 
  with sign0 select                                              
    d3 <= d0 when '0',                                           
    d2 when others;                                              
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      d3r <= d3;                                                 
      sign1 <= sign0;                                            
    end if;                                                      
  end process;                                                   
                                                                 
  u2: penc_23_5 port map (a=>d3r,c=>c1);   
  with d3r select                                                
    nz0 <= '0' when "00000000000000000000000",                                  
           '1' when others;                                      
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      d4 <= d3r(21 downto 0);                        
      c2 <= c1;                                                  
      sign2 <= sign1;                                            
      nz1 <= nz0;                                                
    end if;                                                      
  end process;                                                   
                                                                 
  u3: shift_ftol_22_7                     
            port map (indata=>d4,control=>c2,outdata=>d5);       
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      d6 <= d5;                                                  
      sign3 <= sign2;                                            
      nz2 <= nz1;                                                
      c3 <= c2;                                                  
    end if;                                                      
  end process;                                                   
                                                                 
  u4: table generic map (IN_WIDTH=>7,OUT_WIDTH=>5,TABLE_FILE=>"ftl0.mif") 
            port map(indata=>d6,outdata=>logdata(4 downto 0),clk=>clk);
                                                                 
  with d6 select                                                 
    add <= "00001" when "1111111",                          
           "00001" when "1111110",                        
           "00001" when "1111101",                        
           "00000" when others;                               
                                                                 
  u5: lpm_add_sub generic map (LPM_WIDTH=>5,LPM_DIRECTION=>"ADD")
                  port map(result=>c4,dataa=>c3,datab=>add);     
                                                                 
  logdata(11 downto 10) <= "00";                    
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      logdata(13) <= sign3 ;                          
      logdata(12) <= nz2;                             
      logdata(9 downto 5) <= c4;                    
    end if;                                                      
  end process;                                                   
                                                                 
end rtl;                                                         
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity table is                                                     
  generic(IN_WIDTH: integer := 7;                                   
       OUT_WIDTH: integer := 5;                                     
       TABLE_FILE: string := "ftol.mif");                         
  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);           
       outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        
       clk : in std_logic);                                         
end table;                                                          
                                                                    
architecture rtl of table is                                        
                                                                    
  component lpm_rom                                                 
    generic (LPM_WIDTH: POSITIVE;                                   
             LPM_WIDTHAD: POSITIVE;                                 
          LPM_ADDRESS_CONTROL: STRING;                              
          LPM_FILE: STRING);                                        
   port (address: in std_logic_vector(LPM_WIDTHAD-1 downto 0);      
         outclock: in std_logic;                                    
         q: out std_logic_vector(LPM_WIDTH-1 downto 0));            
  end component;                                                    
                                                                    
begin                                                               
                                                                    
  u1: lpm_rom generic map (LPM_WIDTH=>OUT_WIDTH,                    
                           LPM_WIDTHAD=>IN_WIDTH,                   
                           LPM_ADDRESS_CONTROL=>"UNREGISTERED",   
                           LPM_FILE=>TABLE_FILE)                    
  port map(address=>indata,outclock=>clk,q=>outdata);               
                                                                    
end rtl;                                                            
                                                                    
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity penc_23_5 is                             
port( a : in std_logic_vector(22 downto 0);              
      c : out std_logic_vector(4 downto 0));           
end penc_23_5;                                  
                                                                    
architecture rtl of penc_23_5 is                
begin                                                               
                                                                    
  process(a) begin                                                  
    if(a(22)='1') then                                    
      c <= "10110";                                              
    elsif(a(21)='1') then                                      
      c <= "10101";                                             
    elsif(a(20)='1') then                                      
      c <= "10100";                                             
    elsif(a(19)='1') then                                      
      c <= "10011";                                             
    elsif(a(18)='1') then                                      
      c <= "10010";                                             
    elsif(a(17)='1') then                                      
      c <= "10001";                                             
    elsif(a(16)='1') then                                      
      c <= "10000";                                             
    elsif(a(15)='1') then                                      
      c <= "01111";                                             
    elsif(a(14)='1') then                                      
      c <= "01110";                                             
    elsif(a(13)='1') then                                      
      c <= "01101";                                             
    elsif(a(12)='1') then                                      
      c <= "01100";                                             
    elsif(a(11)='1') then                                      
      c <= "01011";                                             
    elsif(a(10)='1') then                                      
      c <= "01010";                                             
    elsif(a(9)='1') then                                      
      c <= "01001";                                             
    elsif(a(8)='1') then                                      
      c <= "01000";                                             
    elsif(a(7)='1') then                                      
      c <= "00111";                                             
    elsif(a(6)='1') then                                      
      c <= "00110";                                             
    elsif(a(5)='1') then                                      
      c <= "00101";                                             
    elsif(a(4)='1') then                                      
      c <= "00100";                                             
    elsif(a(3)='1') then                                      
      c <= "00011";                                             
    elsif(a(2)='1') then                                      
      c <= "00010";                                             
    elsif(a(1)='1') then                                      
      c <= "00001";                                             
    else                                                            
      c <= "00000";                                               
    end if;                                                         
  end process;                                                      
                                                                    
end rtl;                                                            
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity shift_ftol_22_7 is                       
  port( indata : in std_logic_vector(21 downto 0);       
        control : in std_logic_vector(4 downto 0);     
        outdata : out std_logic_vector(6 downto 0));   
end shift_ftol_22_7;                            
                                                                    
architecture rtl of shift_ftol_22_7 is          
                                                                    
  component lpm_clshift                                             
    generic (LPM_WIDTH : POSITIVE;                                  
             LPM_WIDTHDIST : POSITIVE);                             
    port(data : in std_logic_vector(LPM_WIDTH-1 downto 0);          
         distance : in std_logic_vector(LPM_WIDTHDIST-1 downto 0);  
         direction : in std_logic;                                  
         result : out std_logic_vector(LPM_WIDTH-1 downto 0));      
  end component;                                                    
                                                                    
  signal d0 : std_logic_vector(28 downto 0);    
  signal o0 : std_logic_vector(28 downto 0);    
  signal one : std_logic;                                           
                                                                    
begin                                                               
                                                                    
  one <= '1';                                                       
  d0 <= indata & "0000000";                                        
  u1: lpm_clshift generic map (LPM_WIDTH=>29,LPM_WIDTHDIST=>5)
         port map (data=>d0,distance=>control,result=>o0,direction=>one);
  outdata <= o0(6 downto 0);                           
                                                                    
end rtl;                                                            
                                                            
library ieee;                                               
use ieee.std_logic_1164.all;                                
                                                            
entity pg_pdelay_14 is                               
  generic (PG_WIDTH: integer);                              
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);       
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);     
	   clk : in std_logic);                               
end pg_pdelay_14;                                    
                                                            
architecture rtl of pg_pdelay_14 is                  
                                                            
  signal x0 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x1 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x2 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x3 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x4 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x5 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x6 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x7 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x8 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x9 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x10 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x11 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x12 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x13 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x14 : std_logic_vector(PG_WIDTH-1 downto 0);   
                                                            
begin                                                       
                                                            
  x0 <= x;                                                  
                                                            
  process(clk) begin                                        
    if(clk'event and clk='1') then                          
      x14 <= x13;                                   
      x13 <= x12;                                   
      x12 <= x11;                                   
      x11 <= x10;                                   
      x10 <= x9;                                   
      x9 <= x8;                                   
      x8 <= x7;                                   
      x7 <= x6;                                   
      x6 <= x5;                                   
      x5 <= x4;                                   
      x4 <= x3;                                   
      x3 <= x2;                                   
      x2 <= x1;                                   
      x1 <= x0;                                   
    end if;                                                 
  end process;                                              
                                                            
  y <= x14;                                           
                                                            
end rtl;                                                    
                                                           
library ieee;                                              
use ieee.std_logic_1164.all;                               
                                                           
entity pg_log_shift_1 is                          
  generic (PG_WIDTH: integer);                             
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);      
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);    
	   clk : in std_logic);                              
end pg_log_shift_1;                               
                                                           
architecture rtl of pg_log_shift_1 is             
                                                           
begin                                                      
                                                           
  y <= '0' & x(PG_WIDTH-2) & x(PG_WIDTH-4 downto 0) & '0';     
                                                           
end rtl;                                                   
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_log_unsigned_add_14_5 is            
  port( x,y : in std_logic_vector(13 downto 0);        
        z : out std_logic_vector(13 downto 0);         
        clock : in std_logic);                                     
end pg_log_unsigned_add_14_5;                 
                                                                   
architecture rtl of pg_log_unsigned_add_14_5 is
                                                                   
  component lpm_add_sub                                            
    generic (LPM_WIDTH: integer;                                   
             LPM_DIRECTION: string);                               
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     
  end component;                                                   
                                                                   
  component table                                                  
    generic (IN_WIDTH: integer ;                                   
	     OUT_WIDTH: integer ;                                      
	     TABLE_FILE: string);                                      
  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          
       outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);       
       clk : in std_logic);                                        
  end component;                                                   
                                                                   
  signal x1,y1,xy,yx : std_logic_vector(13 downto 0);  
  signal x2,x3,x4 : std_logic_vector(12 downto 0);     
  signal d0,d1,d4 : std_logic_vector(12 downto 0);     
  signal df0,df1 : std_logic;                                      
  signal z0 : std_logic_vector(12 downto 0);           
  signal d2 : std_logic_vector(5 downto 0);          
  signal sign0,sign1,sign2 : std_logic;                            
  signal signxy : std_logic_vector(1 downto 0);                    
                                                                   
begin                                                              
                                                                   
  x1 <= '0' & x(12 downto 0);                          
  y1 <= '0' & y(12 downto 0);                          
  u1: lpm_add_sub generic map(LPM_WIDTH=>14,LPM_DIRECTION=>"SUB")
       port map(dataa=>x1,datab=>y1,result=>xy);                   
  u2: lpm_add_sub generic map(LPM_WIDTH=>14,LPM_DIRECTION=>"SUB")
       port map(dataa=>y1,datab=>x1,result=>yx);                   
                                                                   
  x2 <= x(12 downto 0) when yx(13)='1' else y(12 downto 0);
  d0 <= xy(12 downto 0) when yx(13)='1' else yx(12 downto 0);
                                                                   
  signxy <= x(13)&y(13);                    
  with signxy select                                               
    sign0 <= y(13) when "01",                        
             x(13) when others;                        
                                                                   
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      x3 <= x2;                                                    
      d1 <= d0;                                                    
      sign1 <= sign0;                                              
    end if;                                                        
  end process;                                                     
                                                                   
  df0 <= '1' when d1(12 downto 8)="00000" else '0';
  u3: table generic map(IN_WIDTH=>8,OUT_WIDTH=>6,TABLE_FILE=>"add0.mif")
            port map(indata=>d1(7 downto 0),outdata=>d2,clk=>clock);
                                                                   
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      df1 <= df0;                                                  
      x4 <= x3;                                                    
      sign2 <= sign1;                                              
    end if;                                                        
  end process;                                                     
                                                                   
  d4(5 downto 0) <= d2 when (df1 = '1') else "000000";
  d4(12 downto 6) <= "0000000";           
                                                                   
  u4: lpm_add_sub generic map(LPM_WIDTH=>13,LPM_DIRECTION=>"ADD")
       port map(dataa=>x4,datab=>d4,result=>z0);                   
                                                                   
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      z(12 downto 0) <= z0;                            
      z(13) <= sign2;                                  
    end if;                                                        
  end process;                                                     
                                                                   
end rtl;                                                           
                                                            
library ieee;                                               
use ieee.std_logic_1164.all;                                
                                                            
entity pg_pdelay_3 is                               
  generic (PG_WIDTH: integer);                              
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);       
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);     
	   clk : in std_logic);                               
end pg_pdelay_3;                                    
                                                            
architecture rtl of pg_pdelay_3 is                  
                                                            
  signal x0 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x1 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x2 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x3 : std_logic_vector(PG_WIDTH-1 downto 0);   
                                                            
begin                                                       
                                                            
  x0 <= x;                                                  
                                                            
  process(clk) begin                                        
    if(clk'event and clk='1') then                          
      x3 <= x2;                                   
      x2 <= x1;                                   
      x1 <= x0;                                   
    end if;                                                 
  end process;                                              
                                                            
  y <= x3;                                           
                                                            
end rtl;                                                    
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_conv_ftol_14_14_5 is         
  port(fixdata : in std_logic_vector(13 downto 0);      
       logdata : out std_logic_vector(13 downto 0);     
       clk : in std_logic);                                        
end pg_conv_ftol_14_14_5;              
                                                                   
architecture rtl of pg_conv_ftol_14_14_5 is 
                                                                   
  component table                                                  
  generic (IN_WIDTH: integer ;                                     
           OUT_WIDTH: integer ;                                    
           TABLE_FILE: string);                                    
  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          
      outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        
      clk : in std_logic);                                         
  end component;                                                   
                                                                   
  component lpm_add_sub                                            
    generic (LPM_WIDTH: integer;                                   
             LPM_DIRECTION: string);                               
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     
  end component;                                                   
                                                                   
  component penc_13_4                        
    port( a : in std_logic_vector(12 downto 0);        
          c : out std_logic_vector(3 downto 0));      
  end component;                                                   
                                                                   
  component shift_ftol_12_7                 
    port( indata : in std_logic_vector(11 downto 0);   
          control : in std_logic_vector(3 downto 0);  
          outdata : out std_logic_vector(6 downto 0));
  end component;                                                   
                                                                   
  signal d1,d0: std_logic_vector(12 downto 0);         
  signal d2: std_logic_vector(12 downto 0);            
  signal d3,d3r: std_logic_vector(12 downto 0);        
  signal one: std_logic_vector(12 downto 0);           
  signal sign: std_logic;                                          
  signal c1: std_logic_vector(3 downto 0);            
  signal d4: std_logic_vector(11 downto 0);            
  signal c2,c3,c4,add: std_logic_vector(3 downto 0);  
  signal d5,d6: std_logic_vector(6 downto 0);         
  signal sign0,sign0r,sign1,sign2,sign3: std_logic;                
  signal nz0,nz1,nz2: std_logic;                                   
                                                                   
begin                                                              
                                                                   
  d1 <=  NOT fixdata(12 downto 0);                     
  one <= "0000000000001";                                              
  u1: lpm_add_sub generic map (LPM_WIDTH=>13,LPM_DIRECTION=>"ADD")
                  port map(result=>d2,dataa=>d1,datab=>one);     
  d0 <= fixdata(12 downto 0);                        
  sign0 <= fixdata(13);                              
                                                                 
  with sign0 select                                              
    d3 <= d0 when '0',                                           
    d2 when others;                                              
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      d3r <= d3;                                                 
      sign1 <= sign0;                                            
    end if;                                                      
  end process;                                                   
                                                                 
  u2: penc_13_4 port map (a=>d3r,c=>c1);   
  with d3r select                                                
    nz0 <= '0' when "0000000000000",                                  
           '1' when others;                                      
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      d4 <= d3r(11 downto 0);                        
      c2 <= c1;                                                  
      sign2 <= sign1;                                            
      nz1 <= nz0;                                                
    end if;                                                      
  end process;                                                   
                                                                 
  u3: shift_ftol_12_7                     
            port map (indata=>d4,control=>c2,outdata=>d5);       
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      d6 <= d5;                                                  
      sign3 <= sign2;                                            
      nz2 <= nz1;                                                
      c3 <= c2;                                                  
    end if;                                                      
  end process;                                                   
                                                                 
  u4: table generic map (IN_WIDTH=>7,OUT_WIDTH=>5,TABLE_FILE=>"ftl1.mif") 
            port map(indata=>d6,outdata=>logdata(4 downto 0),clk=>clk);
                                                                 
  with d6 select                                                 
    add <= "0001" when "1111111",                          
           "0001" when "1111110",                        
           "0001" when "1111101",                        
           "0000" when others;                               
                                                                 
  u5: lpm_add_sub generic map (LPM_WIDTH=>4,LPM_DIRECTION=>"ADD")
                  port map(result=>c4,dataa=>c3,datab=>add);     
                                                                 
  logdata(11 downto 9) <= "000";                    
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      logdata(13) <= sign3 ;                          
      logdata(12) <= nz2;                             
      logdata(8 downto 5) <= c4;                    
    end if;                                                      
  end process;                                                   
                                                                 
end rtl;                                                         
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity penc_13_4 is                             
port( a : in std_logic_vector(12 downto 0);              
      c : out std_logic_vector(3 downto 0));           
end penc_13_4;                                  
                                                                    
architecture rtl of penc_13_4 is                
begin                                                               
                                                                    
  process(a) begin                                                  
    if(a(12)='1') then                                    
      c <= "1100";                                              
    elsif(a(11)='1') then                                      
      c <= "1011";                                             
    elsif(a(10)='1') then                                      
      c <= "1010";                                             
    elsif(a(9)='1') then                                      
      c <= "1001";                                             
    elsif(a(8)='1') then                                      
      c <= "1000";                                             
    elsif(a(7)='1') then                                      
      c <= "0111";                                             
    elsif(a(6)='1') then                                      
      c <= "0110";                                             
    elsif(a(5)='1') then                                      
      c <= "0101";                                             
    elsif(a(4)='1') then                                      
      c <= "0100";                                             
    elsif(a(3)='1') then                                      
      c <= "0011";                                             
    elsif(a(2)='1') then                                      
      c <= "0010";                                             
    elsif(a(1)='1') then                                      
      c <= "0001";                                             
    else                                                            
      c <= "0000";                                               
    end if;                                                         
  end process;                                                      
                                                                    
end rtl;                                                            
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity shift_ftol_12_7 is                       
  port( indata : in std_logic_vector(11 downto 0);       
        control : in std_logic_vector(3 downto 0);     
        outdata : out std_logic_vector(6 downto 0));   
end shift_ftol_12_7;                            
                                                                    
architecture rtl of shift_ftol_12_7 is          
                                                                    
  component lpm_clshift                                             
    generic (LPM_WIDTH : POSITIVE;                                  
             LPM_WIDTHDIST : POSITIVE);                             
    port(data : in std_logic_vector(LPM_WIDTH-1 downto 0);          
         distance : in std_logic_vector(LPM_WIDTHDIST-1 downto 0);  
         direction : in std_logic;                                  
         result : out std_logic_vector(LPM_WIDTH-1 downto 0));      
  end component;                                                    
                                                                    
  signal d0 : std_logic_vector(18 downto 0);    
  signal o0 : std_logic_vector(18 downto 0);    
  signal one : std_logic;                                           
                                                                    
begin                                                               
                                                                    
  one <= '1';                                                       
  d0 <= indata & "0000000";                                        
  u1: lpm_clshift generic map (LPM_WIDTH=>19,LPM_WIDTHDIST=>4)
         port map (data=>d0,distance=>control,result=>o0,direction=>one);
  outdata <= o0(6 downto 0);                           
                                                                    
end rtl;                                                            
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_log_muldiv_1 is                                  
  generic (PG_WIDTH: integer;                                      
           PG_MULDIV: string);                                     
  port( x,y : in std_logic_vector(PG_WIDTH-1 downto 0);            
	   z : out std_logic_vector(PG_WIDTH-1 downto 0);            
	   clk : in std_logic);                                      
end pg_log_muldiv_1;                                       
                                                                   
architecture rtl of pg_log_muldiv_1 is                   
                                                                   
  component lpm_add_sub                                            
    generic (LPM_WIDTH: integer;                                   
             LPM_PIPELINE: integer;                                
             LPM_DIRECTION: string);                               
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  
          clock: in std_logic;                                     
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     
  end component;                                                   
                                                                   
 signal dataa,datab,result: std_logic_vector(PG_WIDTH-3 downto 0); 
                                                                   
begin                                                              
                                                                   
  process(clk) begin                                              
    if(clk'event and clk='1') then                               
      z(PG_WIDTH-1) <= x(PG_WIDTH-1) xor y(PG_WIDTH-1);            
      z(PG_WIDTH-2) <= x(PG_WIDTH-2) and y(PG_WIDTH-2);            
    end if;                                                        
  end process;                                                     
                                                                   
  dataa <= x(PG_WIDTH-3 downto 0);                                 
  datab <= y(PG_WIDTH-3 downto 0);                                 
                                                                   
  u1: lpm_add_sub generic map(LPM_WIDTH=>PG_WIDTH-2,LPM_PIPELINE=>1,
                              LPM_DIRECTION=>PG_MULDIV)           
       port map(dataa=>dataa,datab=>datab,clock=>clk,result=>result);
                                                                   
  z(PG_WIDTH-3 downto 0) <= result;                                
                                                                   
end rtl;                                                           
                                                                   
-- *************************************************************** 
-- * PGPG SIGNED LOGARITHMIC ADDER MODULE                        * 
-- *  AUTHOR: Tsuyoshi Hamada                                    * 
-- *  VERSION: 1.02                                              * 
-- *  LAST MODIFIED AT Fri Jul 12 09:00:01 JST 2002              * 
-- *************************************************************** 
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_log_add_14_5 is                     
  port( x,y : in std_logic_vector(13 downto 0);        
        z : out std_logic_vector(13 downto 0);         
        clock : in std_logic);                                     
end pg_log_add_14_5;                          
                                                                   
architecture rtl of pg_log_add_14_5 is        
                                                                   
  component lpm_add_sub                                            
    generic (LPM_WIDTH: integer;                                   
             LPM_DIRECTION: string);                               
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     
  end component;                                                   
                                                                   
  component lpm_rom                                                
    generic (LPM_WIDTH: POSITIVE;                                  
             LPM_WIDTHAD: POSITIVE;                                
             LPM_ADDRESS_CONTROL: STRING;                          
             LPM_FILE: STRING);                                    
   port (address: in std_logic_vector(LPM_WIDTHAD-1 downto 0);     
         outclock: in std_logic;                                   
         q: out std_logic_vector(LPM_WIDTH-1 downto 0));           
  end component;                                                   
                                                                   
  signal sign_x,sign_y : std_logic;                    
  signal nonz_x,nonz_y : std_logic;                    
  signal x0,y0 : std_logic_vector(12 downto 0);        
  signal x1,y1,xy : std_logic_vector(13 downto 0);     
  signal yx : std_logic_vector(12 downto 0);           
  signal sign_xy : std_logic;                          
  signal x2 : std_logic_vector(12 downto 0);           
  signal x3 : std_logic_vector(12 downto 0);           
  signal x4 : std_logic_vector(12 downto 0);           
  signal x5 : std_logic_vector(12 downto 0);           
  signal d0 : std_logic_vector(12 downto 0);           
  signal d1 : std_logic_vector(12 downto 0);           
  signal df0_plus,df1_plus : std_logic;                
  signal df0_minus,df1_minus : std_logic;              
  signal d2r_msb : std_logic;                          
  signal d2_plus : std_logic_vector(5 downto 0);      
  signal d2_minus : std_logic_vector(7 downto 0);     
  signal d4_plus : std_logic_vector(5 downto 0); 
  signal d4_minus,not_d4_minus,one_for_minusadd,add_at_minus : std_logic_vector(7 downto 0);
  signal allor_of_add_at_minus : std_logic;       
  signal d5_plus : std_logic_vector(12 downto 0); 
  signal d5_minus : std_logic_vector(12 downto 0);
  signal d6,d7 : std_logic_vector(12 downto 0);   
  signal last_add : std_logic_vector(12 downto 0);
  signal signx_eq_signy0 : std_logic;
  signal signx_eq_signy1 : std_logic;
  signal signx_eq_signy2 : std_logic;
  signal signx_ne_signxy : std_logic;
  signal sign_z0 : std_logic;
  signal sign_z1 : std_logic;
  signal sign_z2 : std_logic;
  signal sign_z3 : std_logic;
  signal allor_of_diff : std_logic;
  signal must_not_be_zero0 : std_logic;
  signal must_not_be_zero1 : std_logic;
  signal must_not_be_zero2 : std_logic;
  signal z_logpart : std_logic_vector(11 downto 0);
  signal z_nonzero : std_logic;       
  signal z0 : std_logic_vector(13 downto 0);
  signal z1 : std_logic_vector(13 downto 0);
                                                                   
begin                                                              
                                                                   
  sign_x <= x(13);                                              
  sign_y <= y(13);                                              
  nonz_x <= x(12);                                              
  nonz_y <= y(12);                                              


  -- RESET --
  x0 <= x(12 downto 0) when nonz_x='1' else "0000000000000";
  y0 <= y(12 downto 0) when nonz_y='1' else "0000000000000";

  x1 <= '0' & x0;
  y1 <= '0' & y0;

  -- (Asymmetric) ADDERs --
  u1: lpm_add_sub generic map(LPM_WIDTH=>14,LPM_DIRECTION=>"SUB")
       port map(dataa=>x1,datab=>y1,result=>xy);                   
  u2: lpm_add_sub generic map(LPM_WIDTH=>13,LPM_DIRECTION=>"SUB")
       port map(dataa=>y0,datab=>x0,result=>yx);                   
                                                                   

  sign_xy <= xy(13);                

  -- MULTIPLEXORT (SELECT LOGPART OF THE LAST ADD OPERAND AND DIFF) --
  x2 <= x0 when sign_xy='0' else y0;
  d0 <= xy(12 downto 0) when sign_xy='0' else yx;


  -- SIGN/NONZERO EVALUATION --
  -- XNOR --
  signx_eq_signy0 <= not (sign_x xor sign_y);
  -- XOR --
  signx_ne_signxy <= sign_x xor sign_xy;
  -- MULTIPLEXOR --
  sign_z0 <= signx_ne_signxy when signx_eq_signy0='0' else sign_x;


  ---------------------||/|/|/|/|/|/|/|/|/|/|/ 
  -- PIPELINE (NP>1)   |/|/|/|/|/|/|/|/|/|/|/| 
  ---------------------/|/|/|/|/|/|/|/|/|/|/|| 
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      x3 <= x2;                                                    
      d1 <= d0;                                                    
      sign_z1 <= sign_z0;                                          
      signx_eq_signy1 <= signx_eq_signy0;                          
    end if;                                                        
  end process;                                                     
  
  -- ALL OR (PLUS) --
  df0_plus <= '0' when d1(12 downto 8)="00000" else '1';
  -- ALL OR (MINUS) --
  df0_minus <= '0' when d1(12 downto 8)="00000" else '1';
  
  -- ALL OR -> NOT (PLUS) --
  d2r_msb <= '1' when d1(7 downto 0)="00000000" else '0';

  -- TABLE PLUS (INPUT: 8-bit, OUTPUT: 5-bit) --
  u3: lpm_rom generic map (LPM_WIDTH=>5,          
                           LPM_WIDTHAD=>8,        
                           LPM_ADDRESS_CONTROL=>"UNREGISTERED", 
                           LPM_FILE=>"lap0.mif")     
  port map(address=>d1(7 downto 0),q=>d2_plus(4 downto 0),outclock=>clock);
                                                                   

  -- TABLE MINUS (INPUT: 8-bit, OUTPUT: 8-bit) --
  u4: lpm_rom generic map (LPM_WIDTH=>8,          
                           LPM_WIDTHAD=>8,        
                           LPM_ADDRESS_CONTROL=>"UNREGISTERED", 
                           LPM_FILE=>"lam0.mif")     
    port map(address=>d1(7 downto 0),q=>d2_minus(7 downto 0),outclock=>clock);


  -- SIGN/NONZERO EVALUATION --
  -- ALLOR OF DIFF --
  allor_of_diff <= '0' when d1="0000000000000" else '1';
  -- OR --
  must_not_be_zero0 <= allor_of_diff or signx_eq_signy1;


  ---------------------||/|/|/|/|/|/|/|/|/|/|/ 
  -- PIPELINE (NP>=3)  |/|/|/|/|/|/|/|/|/|/|/| 
  ---------------------/|/|/|/|/|/|/|/|/|/|/|| 

  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      df1_plus <= df0_plus;                                        
      df1_minus <= df0_minus;                                      
      d2_plus(5) <= d2r_msb;                                      
      x4 <= x3;                                                    
      sign_z2 <= sign_z1;                                          
      signx_eq_signy2 <= signx_eq_signy1;                          
      must_not_be_zero1 <= must_not_be_zero0;                      
    end if;                                                        
  end process;                                                     

  -- MULTIPLEXOR (PLUS)  --
  d4_plus <= d2_plus when (df1_plus = '0') else "000000";
  -- SET ZEROS AT HIGHER-BIT OF D5_PLUS  --
  d5_plus <= "0000000" & d4_plus;
  -- MULTIPLEXOR (MINUS) --
  d4_minus <= d2_minus when (df1_minus = '0') else "00000000";

  one_for_minusadd <= "00000001";
  not_d4_minus <= not d4_minus;

  -- ADDER FOR 2'S COMPLEMENT --
  u5: lpm_add_sub generic map(LPM_WIDTH=>8,LPM_DIRECTION=>"ADD")
       port map(dataa=>one_for_minusadd,datab=>not_d4_minus,result=>add_at_minus);
                                                                   

  -- ALLOR OF ADD_AT_MINUS --
  allor_of_add_at_minus <= '0' when add_at_minus="00000000" else '1';
  -- ALLOR OF ADD_AT_MINUS --
  d5_minus(12 downto 8) <= "11111" when allor_of_add_at_minus='1' else "00000";
  d5_minus(7 downto 0) <= add_at_minus;


  -- MULTIPLEXOR (SELECT PLUS_DIFF OR MINUS_DIFF) --
  d6 <= d5_plus when (signx_eq_signy2 = '1') else d5_minus;


  ---------------------||/|/|/|/|/|/|/|/|/|/|/ 
  -- PIPELINE (NP==4)  |/|/|/|/|/|/|/|/|/|/|/| 
  ---------------------/|/|/|/|/|/|/|/|/|/|/|| 

  x5 <= x4;                                                    
  d7 <= d6;                                                    
  must_not_be_zero2 <= must_not_be_zero1;                      
  sign_z3 <= sign_z2;                                          

  -- LAST ADDER --
  u6: lpm_add_sub generic map(LPM_WIDTH=>13,LPM_DIRECTION=>"ADD")
       port map(dataa=>x5,datab=>d7,result=>last_add);

  -- NONZERO EVALUATION --
  z_nonzero <= must_not_be_zero2 and last_add(12);
  -- Z_LOGPART --
  z_logpart <= last_add(11 downto 0);
  -- JOINT Z --
  z0 <= sign_z3 & z_nonzero & z_logpart;


  ---------------------||/|/|/|/|/|/|/|/|/|/|/ 
  -- PIPELINE (NP>=1)  |/|/|/|/|/|/|/|/|/|/|/| 
  ---------------------/|/|/|/|/|/|/|/|/|/|/|| 

  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      z <= z0;                                                     
    end if;                                                        
  end process;                                                     

  -- END --
end rtl;                                                           
                                                            
library ieee;                                               
use ieee.std_logic_1164.all;                                
                                                            
entity pg_pdelay_11 is                               
  generic (PG_WIDTH: integer);                              
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);       
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);     
	   clk : in std_logic);                               
end pg_pdelay_11;                                    
                                                            
architecture rtl of pg_pdelay_11 is                  
                                                            
  signal x0 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x1 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x2 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x3 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x4 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x5 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x6 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x7 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x8 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x9 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x10 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x11 : std_logic_vector(PG_WIDTH-1 downto 0);   
                                                            
begin                                                       
                                                            
  x0 <= x;                                                  
                                                            
  process(clk) begin                                        
    if(clk'event and clk='1') then                          
      x11 <= x10;                                   
      x10 <= x9;                                   
      x9 <= x8;                                   
      x8 <= x7;                                   
      x7 <= x6;                                   
      x6 <= x5;                                   
      x5 <= x4;                                   
      x4 <= x3;                                   
      x3 <= x2;                                   
      x2 <= x1;                                   
      x1 <= x0;                                   
    end if;                                                 
  end process;                                              
                                                            
  y <= x11;                                           
                                                            
end rtl;                                                    
                                                            
library ieee;                                               
use ieee.std_logic_1164.all;                                
                                                            
entity pg_pdelay_1 is                               
  generic (PG_WIDTH: integer);                              
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);       
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);     
	   clk : in std_logic);                               
end pg_pdelay_1;                                    
                                                            
architecture rtl of pg_pdelay_1 is                  
                                                            
  signal x0 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x1 : std_logic_vector(PG_WIDTH-1 downto 0);   
                                                            
begin                                                       
                                                            
  x0 <= x;                                                  
                                                            
  process(clk) begin                                        
    if(clk'event and clk='1') then                          
      x1 <= x0;                                   
    end if;                                                 
  end process;                                              
                                                            
  y <= x1;                                           
                                                            
end rtl;                                                    
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_wtable_14_5 is                        
  port(indata : in std_logic_vector(13 downto 0);       
       outdata : out std_logic_vector(13 downto 0);     
       clk : in std_logic);                                        
end pg_wtable_14_5;                             
                                                                   
architecture rtl of pg_wtable_14_5 is 
                                                                   
  component unreg_table                                            
  generic (IN_WIDTH: integer ;                                     
           OUT_WIDTH: integer ;                                    
           TABLE_FILE: string);                                    
  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          
      outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        
      clk : in std_logic);                                         
  end component;                                                   
                                                                   
  component lpm_add_sub                                            
    generic (LPM_WIDTH: integer;                                   
             LPM_DIRECTION: string);                               
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     
  end component;                                                   
                                                                   
  signal nz,sexp0,sexp: std_logic;                                 
  signal exp: std_logic_vector(3 downto 0);
  signal d10,d2: std_logic_vector(7 downto 0);      
  signal d0,d1,d3,d4,offset0,offset1: std_logic_vector(11 downto 0);
                                                                   
begin                                                              
                                                                   
  nz <= indata(12);                                    
  sexp0 <= indata(11);                                 
  d0 <= "000001000000";                                                 
                                                                   
  u1: lpm_add_sub generic map(LPM_WIDTH=>12,LPM_DIRECTION=>"ADD")
                  port map(result=>d1,dataa=>indata(11 downto 0),datab=>d0);
                                                                   
  sexp <= d1(11);                                      
  exp <= d1(10 downto 7);               
  d10 <= d1(7 downto 0);                            
                                                                   
  u2: unreg_table generic map (IN_WIDTH=>8,OUT_WIDTH=>8,TABLE_FILE=>"wta0.mif") 
                  port map(indata=>d10,outdata=>d2,clk=>clk);      
                                                                   
  d3 <= "0000" & d2;                                           
                                                                   
  --  offset0 = 0x880, offset1 = 0x880 + 0x100        
                                                                   
  offset0 <= "100010000000";                                           
  offset1 <= "100110000000";                                           
                                                                   
  u3: lpm_add_sub generic map (LPM_WIDTH=>12,LPM_DIRECTION=>"ADD")
                   port map(result=>d4,dataa=>d3,datab=>offset0);  
                                                                   
  process(clk) begin                                               
    if(clk'event and clk='1') then                                 
      if(nz = '0') then                                            
        outdata(11 downto 0) <= offset1;               
      else                                                         
        if(sexp = '0') then                                        
          outdata(11 downto 0) <= d4;                  
        else                                                       
          if(exp = "1111") then                                
            outdata(11 downto 0) <= d4;                
          else                                                     
            outdata(11 downto 0) <= offset1;           
          end if;                                                  
        end if;                                                    
      end if;                                                      
    end if;                                                        
  end process;                                                     
                                                                   
  process(clk) begin                                               
    if(clk'event and clk='1') then                                 
      if(nz = '0') then                                            
        outdata(12) <= '1';                            
      else                                                         
        if(sexp = '0') then                                        
          if(exp = "0000") then                                
            if(d10 = "01111111" or                                 
               d10 = "01111110" or                               
               d10 = "01111101" or                               
               d10 = "01111100" or                               
               d10 = "01111011" or                               
               d10 = "01111010" or                               
               d10 = "01111001" or                               
               d10 = "01111000" or                               
               d10 = "01110111" or                               
               d10 = "01110110" or                               
               d10 = "01110101" or                               
               d10 = "01110100") then                            
              outdata(12) <= '0';                      
            else                                                   
              outdata(12) <= '1';                      
            end if;                                                
          else                                                     
            outdata(12) <= '0';                        
          end if;                                                  
        else                                                       
          if(sexp0 ='0') then                                      
            outdata(12) <= '0';                        
          else                                                     
            outdata(12) <= '1';                        
          end if;                                                  
        end if;                                                    
      end if;                                                      
    end if;                                                        
  end process;                                                     
                                                                   
  outdata(13) <= '0';                                  
                                                                   
end rtl;                                                           
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity unreg_table is                                               
  generic(IN_WIDTH: integer := 7;                                   
       OUT_WIDTH: integer := 5;                                     
       TABLE_FILE: string := "ftol.mif");                         
  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);           
       outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        
       clk : in std_logic);                                         
end unreg_table;                                                    
                                                                    
architecture rtl of unreg_table is                                  
                                                                    
  component lpm_rom                                                 
    generic (LPM_WIDTH: POSITIVE;                                   
             LPM_WIDTHAD: POSITIVE;                                 
          LPM_ADDRESS_CONTROL: STRING;                              
          LPM_OUTDATA: STRING;                                      
          LPM_FILE: STRING);                                        
   port (address: in std_logic_vector(LPM_WIDTHAD-1 downto 0);      
         q: out std_logic_vector(LPM_WIDTH-1 downto 0));            
  end component;                                                    
                                                                    
begin                                                               
                                                                    
  u1: lpm_rom generic map (LPM_WIDTH=>OUT_WIDTH,                    
                           LPM_WIDTHAD=>IN_WIDTH,                   
                           LPM_ADDRESS_CONTROL=>"UNREGISTERED",   
                           LPM_OUTDATA=>"UNREGISTERED",           
                           LPM_FILE=>TABLE_FILE)                    
  port map(address=>indata,q=>outdata);                             
                                                                    
end rtl;                                                            
                                                                    
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_dwtable_14_5 is                        
  port(indata : in std_logic_vector(13 downto 0);       
       outdata : out std_logic_vector(13 downto 0);     
       clk : in std_logic);                                        
end pg_dwtable_14_5;                             
                                                                   
architecture rtl of pg_dwtable_14_5 is 
                                                                   
  component unreg_table                                            
  generic (IN_WIDTH: integer ;                                     
           OUT_WIDTH: integer ;                                    
           TABLE_FILE: string);                                    
  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          
      outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        
      clk : in std_logic);                                         
  end component;                                                   
                                                                   
  component lpm_add_sub                                            
    generic (LPM_WIDTH: integer;                                   
             LPM_DIRECTION: string);                               
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     
  end component;                                                   
                                                                   
  signal nz,sexp0,sexp: std_logic;                                 
  signal exp: std_logic_vector(3 downto 0);
  signal d10,d2: std_logic_vector(7 downto 0);      
  signal d0,d1,d3,d4,offset0,offset1: std_logic_vector(11 downto 0);
                                                                   
begin                                                              
                                                                   
  nz <= indata(12);                                    
  sexp0 <= indata(11);                                 
  d0 <= "000001000000";                                                 
                                                                   
  u1: lpm_add_sub generic map(LPM_WIDTH=>12,LPM_DIRECTION=>"ADD")
                  port map(result=>d1,dataa=>indata(11 downto 0),datab=>d0);
                                                                   
  sexp <= d1(11);                                      
  exp <= d1(10 downto 7);               
  d10 <= d1(7 downto 0);                            
                                                                   
  u2: unreg_table generic map (IN_WIDTH=>8,OUT_WIDTH=>8,TABLE_FILE=>"dwt0.mif") 
                  port map(indata=>d10,outdata=>d2,clk=>clk);      
                                                                   
  d3 <= "0000" & d2;                                           
                                                                   
  --  offset0 = 0xe00, offset1 = 0xe00 + 0x100        
                                                                   
  offset0 <= "111000000000";                                           
  offset1 <= "111100000000";                                           
                                                                   
  u3: lpm_add_sub generic map (LPM_WIDTH=>12,LPM_DIRECTION=>"ADD")
                   port map(result=>d4,dataa=>d3,datab=>offset0);  
                                                                   
  process(clk) begin                                               
    if(clk'event and clk='1') then                                 
      if(nz = '0') then                                            
        outdata(11 downto 0) <= offset1;               
      else                                                         
        if(sexp = '0') then                                        
          outdata(11 downto 0) <= d4;                  
        else                                                       
          if(exp = "1111") then                                
            outdata(11 downto 0) <= d4;                
          else                                                     
            outdata(11 downto 0) <= offset1;           
          end if;                                                  
        end if;                                                    
      end if;                                                      
    end if;                                                        
  end process;                                                     
                                                                   
  process(clk) begin                                               
    if(clk'event and clk='1') then                                 
      if(nz = '0') then                                            
        outdata(12) <= '1';                            
      else                                                         
        if(sexp = '0') then                                        
          if(exp = "0000") then                                
            if(d10 = "01111111" or                                 
               d10 = "01111110" or                               
               d10 = "01111101" or                               
               d10 = "01111100" or                               
               d10 = "01111011" or                               
               d10 = "01111010" or                               
               d10 = "01111001" or                               
               d10 = "01111000") then                            
              outdata(12) <= '0';                      
            else                                                   
              outdata(12) <= '1';                      
            end if;                                                
          else                                                     
            outdata(12) <= '0';                        
          end if;                                                  
        else                                                       
          if(sexp0 ='0') then                 
            outdata(12) <= '0';                        
          else                                                     
            outdata(12) <= '1';                        
          end if;                                                  
        end if;                                                    
      end if;                                                      
    end if;                                                        
  end process;                                                     
                                                                   
  outdata(13) <= '1';                                  
                                                                   
end rtl;                                                           
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_log_smuldiv_1_14 is                         
  generic (PG_WIDTH: integer;                                      
           PG_MULDIV: string);                                     
  port( x,y : in std_logic_vector(PG_WIDTH-1 downto 0);            
	   z : out std_logic_vector(PG_WIDTH-1 downto 0);            
	   clk : in std_logic);                                      
end pg_log_smuldiv_1_14;                               
                                                                   
architecture rtl of pg_log_smuldiv_1_14 is          
                                                                   
  component lpm_add_sub                                            
    generic (LPM_WIDTH: integer;                                   
             LPM_PIPELINE: integer;                                
             LPM_DIRECTION: string);                               
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  
          clock: in std_logic;                                     
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     
  end component;                                                   
                                                                   
 signal dataa,datab,result: std_logic_vector(PG_WIDTH-2 downto 0); 
                                                                   
begin                                                              
                                                                   
  process(clk) begin                                              
    if(clk'event and clk='1') then                               
      z(PG_WIDTH-1) <= x(PG_WIDTH-1) xor y(PG_WIDTH-1);            
      z(PG_WIDTH-2) <= x(PG_WIDTH-2) and y(PG_WIDTH-2);            
    end if;                                                        
  end process;                                                     
                                                                   
  dataa <= '0' & x(PG_WIDTH-3 downto 0);                                 
  datab <= '0' & y(PG_WIDTH-3 downto 0);                                 
                                                                   
  u1: lpm_add_sub generic map(LPM_WIDTH=>PG_WIDTH-1,LPM_PIPELINE=>1,
                              LPM_DIRECTION=>PG_MULDIV)           
       port map(dataa=>dataa,datab=>datab,clock=>clk,result=>result);
                                                                   
  with result(PG_WIDTH-2) select                                                                  
    z(PG_WIDTH-3 downto 0) <= "000000000000" when '1',     
                           result(PG_WIDTH-3 downto 0) when others;
                                                                   
end rtl;                                                           
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_conv_ltof_14_5_57 is        
  port(logdata : in std_logic_vector(13 downto 0);
       fixdata : out std_logic_vector(56 downto 0);
       clk : in std_logic);                                        
end pg_conv_ltof_14_5_57;             
                                                                   
architecture rtl of pg_conv_ltof_14_5_57 is 
                                                                   
  component table                                                  
    generic (IN_WIDTH: integer ;                                   
             OUT_WIDTH: integer ;                                  
             TABLE_FILE: string);                                  
    port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);        
         outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);     
         clk : in std_logic);                                      
  end component;                                                   
                                                                   
  component shift_ltof_6_56                 
    port( indata : in std_logic_vector(5 downto 0);   
          control : in std_logic_vector(6 downto 0);
          clk : in std_logic;                                      
          outdata : out std_logic_vector(55 downto 0)); 
  end component;                                                   
                                                                   
  signal frac1 : std_logic_vector(4 downto 0);        
  signal frac2 : std_logic_vector(5 downto 0);        
  signal sign1 : std_logic;                                        
  signal nz1 : std_logic;                                          
  signal exp1 : std_logic_vector(6 downto 0);          
  signal fix1 : std_logic_vector(55 downto 0);          
                                                                   
begin                                                              
                                                                   
  process(clk) begin                                               
    if(clk'event and clk='1') then                                 
      sign1 <= logdata(13);                             
      nz1 <= logdata(12);                               
      exp1 <= logdata(11 downto 5);            
    end if;                                                        
  end process;                                                     
                                                                   
  u1: table generic map (IN_WIDTH=>5,OUT_WIDTH=>5,TABLE_FILE=>"ltf0.mif")
          port map(indata=>logdata(4 downto 0),outdata=>frac1,clk=>clk);
                                                                   
  with nz1 select                                                  
    frac2 <= '1' & frac1 when '1',                                 
             "000000" when others;                                
                                                                   
  u3: shift_ltof_6_56                       
            port map (indata=>frac2,control=>exp1,outdata=>fix1,clk=>clk);
                                                                   
  process(clk) begin                                               
    if(clk'event and clk='1') then                                 
      fixdata(56) <= sign1;                             
      fixdata(55 downto 0) <= fix1;                     
    end if;                                                        
  end process;                                                     
                                                                   
end rtl;                                                           
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity shift_ltof_6_56 is                        
  port( indata : in std_logic_vector(5 downto 0);       
        control : in std_logic_vector(6 downto 0);     
        clk : in std_logic;                                         
        outdata : out std_logic_vector(55 downto 0));   
end shift_ltof_6_56;                             
                                                                    
architecture rtl of shift_ltof_6_56 is
                                                                    
  component lpm_clshift                                             
    generic (LPM_WIDTH : POSITIVE;                                  
             LPM_WIDTHDIST : POSITIVE);                             
    port(data : in std_logic_vector(LPM_WIDTH-1 downto 0);          
         distance : in std_logic_vector(LPM_WIDTHDIST-1 downto 0);  
         result : out std_logic_vector(LPM_WIDTH-1 downto 0));      
  end component;                                                    
                                                                    
  signal d0,o0 : std_logic_vector(65 downto 0);          
                                                                    
begin                                                               
                                                                    
  d0 <= "000000000000000000000000000000000000000000000000000000000000" & indata;                                        
  u1: lpm_clshift generic map (LPM_WIDTH=>66,LPM_WIDTHDIST=>6)
          port map (data=>d0,distance=>control(5 downto 0),result=>o0);
  outdata <= o0(60 downto 5);                
                                                                    
end rtl;                                                            
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
use ieee.std_logic_unsigned.all;                                    
                                                                    
entity pg_fix_accum_57_64_2 is                     
  port (fdata: in std_logic_vector(56 downto 0);      
        sdata0: out std_logic_vector(63 downto 0);     
        sdata1: out std_logic_vector(63 downto 0);     
        sdata2: out std_logic_vector(63 downto 0);     
        run : in std_logic;                                         
        clk : in std_logic);                                        
end pg_fix_accum_57_64_2;                           
                                                                    
architecture rtl of pg_fix_accum_57_64_2 is        
                                                                    
  component lpm_add_sub                                             
    generic (LPM_WIDTH: integer);                                   
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);   
          cin : in std_logic;                                       
          add_sub : in std_logic;                                   
          cout : out std_logic;                                     
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));      
  end component;                                                    
                                                                    
  signal fx: std_logic_vector(63 downto 0);               
  signal run2,run1 : std_logic;                                   
  signal signx,signx1,cx1,cx2: std_logic;                         
  signal fxl,sxl1,sxl2: std_logic_vector(31 downto 0);            
  signal fxh,fxh1,sxh1,sxh2 : std_logic_vector(31 downto 0);      
  signal sxl20: std_logic_vector(31 downto 0);   
  signal sxh20: std_logic_vector(31 downto 0);   
  signal sxl21: std_logic_vector(31 downto 0);   
  signal sxh21: std_logic_vector(31 downto 0);   
  signal sxl22: std_logic_vector(31 downto 0);   
  signal sxh22: std_logic_vector(31 downto 0);   
  signal count,count2: std_logic_vector(3 downto 0);              
                                                                  
begin                                                             
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      run2 <= run1;                                               
      run1 <= run;                                                
    end if;                                                       
  end process;                                                    
                                                                  
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run1 = '1') then                                         
        if(count = "0010") then                               
          count <= "0000";                                      
        else                                                      
          count <= count + "0001";                              
        end if;                                                   
      else                                                        
        if(run = '1') then                                        
          count <= "0000";                                      
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run2 = '1') then                                         
        if(count2 = "0010") then                              
          count2 <= "0000";                                     
        else                                                      
          count2 <= count2 + "0001";                            
        end if;                                                   
      else                                                        
        if(run1 = '1') then                                       
          count2 <= "0000";                                     
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  fx <= "00000000" & fdata(55 downto 0);                              
  signx <= not fdata(56);                                         
  fxl <= fx(31 downto 0);                                         
  fxh <= fx(63 downto 32);                                        
                                                                  
  u1: lpm_add_sub generic map(LPM_WIDTH=>32)                      
      port map(dataa=>sxl2,                                       
               datab=>fxl,add_sub=>signx,                         
               cin=>fdata(56),                                    
               cout=>cx1,result=>sxl1);                           
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      cx2 <= cx1;                                                 
      fxh1 <= fxh;                                                
      signx1 <= signx;                                            
    end if;                                                       
  end process;                                                    
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run1 = '1') then                                         
        if(count = "0000") then                               
          sxl20 <= sxl1;                                    
        end if;                                                   
      else                                                        
        if(run = '1') then                                        
          sxl20 <= "00000000000000000000000000000000";
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  sdata0(31 downto 0) <= sxl20;               
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run1 = '1') then                                         
        if(count = "0001") then                               
          sxl21 <= sxl1;                                    
        end if;                                                   
      else                                                        
        if(run = '1') then                                        
          sxl21 <= "00000000000000000000000000000000";
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  sdata1(31 downto 0) <= sxl21;               
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run1 = '1') then                                         
        if(count = "0010") then                               
          sxl22 <= sxl1;                                    
        end if;                                                   
      else                                                        
        if(run = '1') then                                        
          sxl22 <= "00000000000000000000000000000000";
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  sdata2(31 downto 0) <= sxl22;               
  with count select                                             
    sxl2 <= sxl20 when "0000",                                
            sxl21 when "0001",                       
            sxl22 when "0010",                       
            sxl20 when others;                                   
                                                               
  u2: lpm_add_sub generic map(LPM_WIDTH=>32)                      
      port map(dataa=>sxh2,                                       
               datab=>fxh1,add_sub=>signx1,                       
               cin=>cx2,result=>sxh1);                            
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run2 = '1') then                                         
        if(count2 = "0000") then                              
          sxh20 <= sxh1;                                    
        end if;                                                   
      else                                                        
        if(run1 = '1') then                                       
          sxh20 <= "00000000000000000000000000000000";
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  sdata0(63 downto 32) <= sxh20;        
                                                                  
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run2 = '1') then                                         
        if(count2 = "0001") then                              
          sxh21 <= sxh1;                                    
        end if;                                                   
      else                                                        
        if(run1 = '1') then                                       
          sxh21 <= "00000000000000000000000000000000";
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  sdata1(63 downto 32) <= sxh21;        
                                                                  
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run2 = '1') then                                         
        if(count2 = "0010") then                              
          sxh22 <= sxh1;                                    
        end if;                                                   
      else                                                        
        if(run1 = '1') then                                       
          sxh22 <= "00000000000000000000000000000000";
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  sdata2(63 downto 32) <= sxh22;        
                                                                  
  with count2 select                                             
    sxh2 <= sxh20 when "0000",                                 
            sxh21 when "0001",                       
            sxh22 when "0010",                       
            sxh20 when others;                                   
                                                                    
end rtl;                                                            
                                                            
library ieee;                                               
use ieee.std_logic_1164.all;                                
                                                            
entity pg_pdelay_5 is                               
  generic (PG_WIDTH: integer);                              
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);       
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);     
	   clk : in std_logic);                               
end pg_pdelay_5;                                    
                                                            
architecture rtl of pg_pdelay_5 is                  
                                                            
  signal x0 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x1 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x2 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x3 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x4 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x5 : std_logic_vector(PG_WIDTH-1 downto 0);   
                                                            
begin                                                       
                                                            
  x0 <= x;                                                  
                                                            
  process(clk) begin                                        
    if(clk'event and clk='1') then                          
      x5 <= x4;                                   
      x4 <= x3;                                   
      x3 <= x2;                                   
      x2 <= x1;                                   
      x1 <= x0;                                   
    end if;                                                 
  end process;                                              
                                                            
  y <= x5;                                           
                                                            
end rtl;                                                    
                                                            
library ieee;                                               
use ieee.std_logic_1164.all;                                
                                                            
entity pg_pdelay_13 is                               
  generic (PG_WIDTH: integer);                              
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);       
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);     
	   clk : in std_logic);                               
end pg_pdelay_13;                                    
                                                            
architecture rtl of pg_pdelay_13 is                  
                                                            
  signal x0 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x1 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x2 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x3 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x4 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x5 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x6 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x7 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x8 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x9 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x10 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x11 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x12 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x13 : std_logic_vector(PG_WIDTH-1 downto 0);   
                                                            
begin                                                       
                                                            
  x0 <= x;                                                  
                                                            
  process(clk) begin                                        
    if(clk'event and clk='1') then                          
      x13 <= x12;                                   
      x12 <= x11;                                   
      x11 <= x10;                                   
      x10 <= x9;                                   
      x9 <= x8;                                   
      x8 <= x7;                                   
      x7 <= x6;                                   
      x6 <= x5;                                   
      x5 <= x4;                                   
      x4 <= x3;                                   
      x3 <= x2;                                   
      x2 <= x1;                                   
      x1 <= x0;                                   
    end if;                                                 
  end process;                                              
                                                            
  y <= x13;                                           
                                                            
end rtl;                                                    
                                                            
library ieee;                                               
use ieee.std_logic_1164.all;                                
                                                            
entity pg_pdelay_2 is                               
  generic (PG_WIDTH: integer);                              
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);       
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);     
	   clk : in std_logic);                               
end pg_pdelay_2;                                    
                                                            
architecture rtl of pg_pdelay_2 is                  
                                                            
  signal x0 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x1 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x2 : std_logic_vector(PG_WIDTH-1 downto 0);   
                                                            
begin                                                       
                                                            
  x0 <= x;                                                  
                                                            
  process(clk) begin                                        
    if(clk'event and clk='1') then                          
      x2 <= x1;                                   
      x1 <= x0;                                   
    end if;                                                 
  end process;                                              
                                                            
  y <= x2;                                           
                                                            
end rtl;                                                    
