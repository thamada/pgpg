
--+-------------------------+
--| PGPG Fixed-Point Sub    |
--|      by Tsuyoshi Hamada |
--+-------------------------+
library ieee;
use ieee.std_logic_1164.all;

entity pg_fix_sub_32_1 is
  port(x,y : in std_logic_vector(31 downto 0);
       z : out std_logic_vector(31 downto 0);
       clk : in std_logic);
end pg_fix_sub_32_1;
architecture rtl of pg_fix_sub_32_1 is
  component pg_adder_RCA_SUB_32_1
    port (x,y: in std_logic_vector(31 downto 0);
          z:  out std_logic_vector(31 downto 0);
          clk: in std_logic);
  end component;
begin
  u0: pg_adder_RCA_SUB_32_1
      port map(x=>x,y=>y,z=>z,clk=>clk);
end rtl;

--+--------------------------+
--| PGPG Ripple-Carry Adder  |
--|  2003/10/31              |
--|      by Tsuyoshi Hamada  |
--+--------------------------+
library ieee;
use ieee.std_logic_1164.all;

entity pg_adder_RCA_SUB_32_1 is
  port(x,y : in std_logic_vector(31 downto 0);
       z : out std_logic_vector(31 downto 0);
       clk : in std_logic);
end pg_adder_RCA_SUB_32_1;

architecture rtl of pg_adder_RCA_SUB_32_1 is
  component pg_lcell_ari                    
    generic (MASK: string := "96E8";      
             FF: integer  := 1);            
    port (x,y,ci,clk: in std_logic;         
          z,co: out std_logic);             
  end component;                           

  signal co : std_logic_vector(30 downto 0);
begin
-- ADDER: (32-b)*1 + (33-b)*0
-- [ADD: MASK="96E8", u0:ci='0'],[SUB: MASK="69B2", u0:ci='1']
--  0-stage (32-bit)--
  u0: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(0),y=>y(0),ci=>'1',clk=>clk,z=>z(0),co=>co(0));
  u1: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(1),y=>y(1),ci=>co(0),clk=>clk,z=>z(1),co=>co(1));
  u2: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(2),y=>y(2),ci=>co(1),clk=>clk,z=>z(2),co=>co(2));
  u3: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(3),y=>y(3),ci=>co(2),clk=>clk,z=>z(3),co=>co(3));
  u4: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(4),y=>y(4),ci=>co(3),clk=>clk,z=>z(4),co=>co(4));
  u5: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(5),y=>y(5),ci=>co(4),clk=>clk,z=>z(5),co=>co(5));
  u6: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(6),y=>y(6),ci=>co(5),clk=>clk,z=>z(6),co=>co(6));
  u7: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(7),y=>y(7),ci=>co(6),clk=>clk,z=>z(7),co=>co(7));
  u8: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(8),y=>y(8),ci=>co(7),clk=>clk,z=>z(8),co=>co(8));
  u9: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(9),y=>y(9),ci=>co(8),clk=>clk,z=>z(9),co=>co(9));
  u10: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(10),y=>y(10),ci=>co(9),clk=>clk,z=>z(10),co=>co(10));
  u11: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(11),y=>y(11),ci=>co(10),clk=>clk,z=>z(11),co=>co(11));
  u12: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(12),y=>y(12),ci=>co(11),clk=>clk,z=>z(12),co=>co(12));
  u13: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(13),y=>y(13),ci=>co(12),clk=>clk,z=>z(13),co=>co(13));
  u14: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(14),y=>y(14),ci=>co(13),clk=>clk,z=>z(14),co=>co(14));
  u15: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(15),y=>y(15),ci=>co(14),clk=>clk,z=>z(15),co=>co(15));
  u16: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(16),y=>y(16),ci=>co(15),clk=>clk,z=>z(16),co=>co(16));
  u17: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(17),y=>y(17),ci=>co(16),clk=>clk,z=>z(17),co=>co(17));
  u18: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(18),y=>y(18),ci=>co(17),clk=>clk,z=>z(18),co=>co(18));
  u19: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(19),y=>y(19),ci=>co(18),clk=>clk,z=>z(19),co=>co(19));
  u20: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(20),y=>y(20),ci=>co(19),clk=>clk,z=>z(20),co=>co(20));
  u21: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(21),y=>y(21),ci=>co(20),clk=>clk,z=>z(21),co=>co(21));
  u22: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(22),y=>y(22),ci=>co(21),clk=>clk,z=>z(22),co=>co(22));
  u23: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(23),y=>y(23),ci=>co(22),clk=>clk,z=>z(23),co=>co(23));
  u24: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(24),y=>y(24),ci=>co(23),clk=>clk,z=>z(24),co=>co(24));
  u25: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(25),y=>y(25),ci=>co(24),clk=>clk,z=>z(25),co=>co(25));
  u26: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(26),y=>y(26),ci=>co(25),clk=>clk,z=>z(26),co=>co(26));
  u27: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(27),y=>y(27),ci=>co(26),clk=>clk,z=>z(27),co=>co(27));
  u28: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(28),y=>y(28),ci=>co(27),clk=>clk,z=>z(28),co=>co(28));
  u29: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(29),y=>y(29),ci=>co(28),clk=>clk,z=>z(29),co=>co(29));
  u30: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(30),y=>y(30),ci=>co(29),clk=>clk,z=>z(30),co=>co(30));
  u31: pg_lcell_ari generic map (MASK=>"69B2", FF=>1)
     port map(x=>x(31),y=>y(31),ci=>co(30),clk=>clk,z=>z(31));
end rtl;
-- *************************************************************** 
-- * PGPG FIXED-POINT TO LOGARITHMIC FORMAT CONVERTER            * 
-- *  AUTHOR: Tsuyoshi Hamada                                    * 
-- *  VERSION: 2.00                                              * 
-- *  LAST MODIFIED AT Tue Jun 03 22:52:01 JST 2003              * 
-- *************************************************************** 
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_conv_ftol_32_17_8_4 is         
  port(fixdata : in std_logic_vector(31 downto 0);      
       logdata : out std_logic_vector(16 downto 0);     
       clk : in std_logic);                                        
end pg_conv_ftol_32_17_8_4;              
                                                                   
architecture rtl of pg_conv_ftol_32_17_8_4 is 
                                                                   
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
                                                                   
  component penc_31_5                        
    port( a : in std_logic_vector(30 downto 0);        
          c : out std_logic_vector(4 downto 0));      
  end component;                                                   
                                                                   
  component shift_ftol_30_10                 
    port( indata : in std_logic_vector(29 downto 0);   
          control : in std_logic_vector(4 downto 0);  
          outdata : out std_logic_vector(9 downto 0));
  end component;                                                   
                                                                   
  signal d1,d0: std_logic_vector(30 downto 0);         
  signal d2: std_logic_vector(30 downto 0);            
  signal d3,d3r: std_logic_vector(30 downto 0);        
  signal one: std_logic_vector(30 downto 0);           
  signal sign: std_logic;                                          
  signal c1: std_logic_vector(4 downto 0);            
  signal d4: std_logic_vector(29 downto 0);            
  signal c2,c3,c4,add: std_logic_vector(4 downto 0);  
  signal d5,d6: std_logic_vector(9 downto 0);         
  signal sign0,sign0r,sign1,sign2,sign3: std_logic;                
  signal nz0,nz1,nz2: std_logic;                                   
                                                                   
begin                                                              
                                                                   
  d1 <=  NOT fixdata(30 downto 0);                     
  one <= "0000000000000000000000000000001";                                              
  u1: lpm_add_sub generic map (LPM_WIDTH=>31,LPM_DIRECTION=>"ADD")
                  port map(result=>d2,dataa=>d1,datab=>one);     
  d0 <= fixdata(30 downto 0);                        
  sign0 <= fixdata(31);                              
                                                                 
  with sign0 select                                              
    d3 <= d0 when '0',                                           
    d2 when others;                                              
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      d3r <= d3;                                                 
      sign1 <= sign0;                                            
    end if;                                                      
  end process;                                                   
                                                                 
  u2: penc_31_5 port map (a=>d3r,c=>c1);   
  with d3r select                                                
    nz0 <= '0' when "0000000000000000000000000000000",                                  
           '1' when others;                                      
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      d4 <= d3r(29 downto 0);                        
      c2 <= c1;                                                  
      sign2 <= sign1;                                            
      nz1 <= nz0;                                                
    end if;                                                      
  end process;                                                   
                                                                 
  u3: shift_ftol_30_10                     
            port map (indata=>d4,control=>c2,outdata=>d5);       
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      d6 <= d5;                                                  
      sign3 <= sign2;                                            
      nz2 <= nz1;                                                
      c3 <= c2;                                                  
    end if;                                                      
  end process;                                                   
                                                                 
  u4: table generic map(IN_WIDTH=>10,OUT_WIDTH=>8,TABLE_FILE=>"ftl0.mif") 
            port map(indata=>d6,outdata=>logdata(7 downto 0),clk=>clk);

  with d6 select                                                 
    add <= "00001" when "1111111111",                          
           "00001" when "1111111110",                        
           "00001" when "1111111101",                        
           "00000" when others;                               
                                                                 
  u5: lpm_add_sub generic map (LPM_WIDTH=>5,LPM_DIRECTION=>"ADD")
                  port map(result=>c4,dataa=>c3,datab=>add);     
                                                                 
  logdata(14 downto 13) <= "00";                    
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      logdata(16) <= sign3 ;                          
      logdata(15) <= nz2;                             
      logdata(12 downto 8) <= c4;                    
    end if;                                                      
  end process;                                                   
                                                                 
end rtl;                                                         
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity penc_31_5 is                             
port( a : in std_logic_vector(30 downto 0);              
      c : out std_logic_vector(4 downto 0));           
end penc_31_5;                                  
                                                                    
architecture rtl of penc_31_5 is                
begin                                                               
                                                                    
  process(a) begin                                                  
    if(a(30)='1') then                                    
      c <= "11110";                                              
    elsif(a(29)='1') then                                      
      c <= "11101";                                             
    elsif(a(28)='1') then                                      
      c <= "11100";                                             
    elsif(a(27)='1') then                                      
      c <= "11011";                                             
    elsif(a(26)='1') then                                      
      c <= "11010";                                             
    elsif(a(25)='1') then                                      
      c <= "11001";                                             
    elsif(a(24)='1') then                                      
      c <= "11000";                                             
    elsif(a(23)='1') then                                      
      c <= "10111";                                             
    elsif(a(22)='1') then                                      
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
                                                                    
entity shift_ftol_30_10 is                       
  port( indata : in std_logic_vector(29 downto 0);       
        control : in std_logic_vector(4 downto 0);     
        outdata : out std_logic_vector(9 downto 0));   
end shift_ftol_30_10;                            
                                                                    
architecture rtl of shift_ftol_30_10 is          
                                                                    
  component lpm_clshift                                             
    generic (LPM_WIDTH : POSITIVE;                                  
             LPM_WIDTHDIST : POSITIVE);                             
    port(data : in std_logic_vector(LPM_WIDTH-1 downto 0);          
         distance : in std_logic_vector(LPM_WIDTHDIST-1 downto 0);  
         direction : in std_logic;                                  
         result : out std_logic_vector(LPM_WIDTH-1 downto 0));      
  end component;                                                    
                                                                    
  signal d0 : std_logic_vector(39 downto 0);    
  signal o0 : std_logic_vector(39 downto 0);    
  signal one : std_logic;                                           
                                                                    
begin                                                               
                                                                    
  one <= '1';                                                       
  d0 <= indata & "0000000000";                                        
  u1: lpm_clshift generic map (LPM_WIDTH=>40,LPM_WIDTHDIST=>5)
         port map (data=>d0,distance=>control,result=>o0,direction=>one);
  outdata <= o0(9 downto 0);                           
                                                                    
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

entity pg_pdelay_8 is
  generic (PG_WIDTH: integer);
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);
	   clk : in std_logic);
end pg_pdelay_8;

architecture rtl of pg_pdelay_8 is

  signal x0 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x1 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x2 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x3 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x4 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x5 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x6 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x7 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x8 : std_logic_vector(PG_WIDTH-1 downto 0);   

begin

  x0 <= x;

  process(clk) begin
    if(clk'event and clk='1') then
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

  y <= x8;

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
                                                                   
-- *************************************************************** 
-- * PGPG UNSIGNED LOGARITHMIC ADDER MODULE                      * 
-- *  AUTHOR: Tsuyoshi Hamada                                    * 
-- *  VERSION: 3.00                                              * 
-- *  LAST MODIFIED AT Tue Jun 03 10:52:01 JST 2003              * 
-- *************************************************************** 
library ieee;
use ieee.std_logic_1164.all;

entity pg_log_unsigned_add_17_8_3 is
  port( x,y : in std_logic_vector(16 downto 0);
        z : out std_logic_vector(16 downto 0);
        clock : in std_logic);
end pg_log_unsigned_add_17_8_3;

architecture rtl of pg_log_unsigned_add_17_8_3 is

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
                                                                   
  signal x1,y1,xy,yx : std_logic_vector(16 downto 0);  
  signal xy0,yx0 : std_logic_vector(16 downto 0);  
  signal x0,y0 : std_logic_vector(16 downto 0);  
  signal x2,x3,x3r,x4 : std_logic_vector(15 downto 0);     
  signal d0,d1,d4 : std_logic_vector(15 downto 0);     
  signal df0,df0r,df1 : std_logic;                                      
  signal z0 : std_logic_vector(15 downto 0);           
  signal d2r_msb,d2r_msbr : std_logic;                         
  signal d2 : std_logic_vector(8 downto 0);          
  signal sign0,sign1,sign1r,sign2 : std_logic;                            
  signal signxy : std_logic_vector(1 downto 0);                    
                                                                   
begin                                                              
                                                                   
  x1 <= '0' & x(15 downto 0);                          
  y1 <= '0' & y(15 downto 0);                          
  u1: lpm_add_sub generic map(LPM_WIDTH=>17,LPM_DIRECTION=>"SUB")
       port map(dataa=>x1,datab=>y1,result=>xy);                   
  u2: lpm_add_sub generic map(LPM_WIDTH=>17,LPM_DIRECTION=>"SUB")
       port map(dataa=>y1,datab=>x1,result=>yx);                   
                                                                   
  -- PIPELINE 1 (OFF) -----------------
  xy0 <= xy;                                                   
  yx0 <= yx;                                                   
  x0 <= x;                                                     
  y0 <= y;                                                     
  ------------------------------------.
                                                                   
  x2 <= x0(15 downto 0) when yx0(16)='1' else y0(15 downto 0);
  d0 <= xy0(15 downto 0) when yx0(16)='1' else yx0(15 downto 0);
                                                                   
  signxy <= x0(16)&y0(16);                    
  with signxy select                                               
    sign0 <= y0(16) when "01",                        
             x0(16) when others;                        
                                                                   
  -- PIPELINE 2 -----------------------
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      x3 <= x2;                                                    
      d1 <= d0;                                                    
      sign1 <= sign0;                                              
    end if;                                                        
  end process;                                                     
  ------------------------------------.
                                                                   
  df0 <= '1' when d1(15 downto 12)="0000" else '0';
  
  -- ALL OR -> NOT (PLUS) --
  d2r_msb <= '1' when d1(11 downto 0)="000000000000" else '0';

  u3: table generic map(IN_WIDTH=>12,OUT_WIDTH=>8,TABLE_FILE=>"add0.mif")
            port map(indata=>d1(11 downto 0),outdata=>d2(7 downto 0),clk=>clock);
                                                                   
  -- PIPELINE 3 -----------------------
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      df0r <= df0;                                                  
      x3r <= x3;                                                    
      d2r_msbr <= d2r_msb;                                           
      sign1r <= sign1;                                              
    end if;                                                        
  end process;                                                     
  ------------------------------------.
                                                                   
  -- PIPELINE 4 (OFF) -----------------
  df1 <= df0r;                                                   
  x4 <= x3r;                                                     
  d2(8) <= d2r_msbr;                                            
  sign2 <= sign1r;                                               
  ------------------------------------.
                                                                   
  d4(8 downto 0) <= d2 when (df1 = '1') else "000000000";
  d4(15 downto 9) <= "0000000";           
                                                                   
  u4: lpm_add_sub generic map(LPM_WIDTH=>16,LPM_DIRECTION=>"ADD")
       port map(dataa=>x4,datab=>d4,result=>z0);                   
                                                                   
  -- PIPELINE 4 -----------------------
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      z(15 downto 0) <= z0;                            
      z(16) <= sign2;                                  
    end if;                                                        
  end process;                                                     
  ------------------------------------.
                                                                   
end rtl;                                                           
                                                           
library ieee;                                              
use ieee.std_logic_1164.all;                               
                                                           
entity pg_log_shift_m1 is                          
  generic (PG_WIDTH: integer);                             
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);      
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);    
	   clk : in std_logic);                              
end pg_log_shift_m1;                               
                                                           
architecture rtl of pg_log_shift_m1 is             
                                                           
begin                                                      
                                                           
  y <= '0' & x(PG_WIDTH-2) & '0' & x(PG_WIDTH-3 downto 1);   
                                                           
end rtl;                                                   

library ieee;
use ieee.std_logic_1164.all;

entity pg_log_mul_17_1 is
  port( x,y : in std_logic_vector(16 downto 0);
          z : out std_logic_vector(16 downto 0);
        clk : in std_logic);
end pg_log_mul_17_1;

architecture rtl of pg_log_mul_17_1 is

  component lpm_add_sub
    generic (LPM_WIDTH: integer;
             LPM_PIPELINE: integer;
             LPM_DIRECTION: string);
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);
          clock: in std_logic;
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));
  end component;

 signal addx,addy,addz: std_logic_vector(14 downto 0); 

begin

  process(clk) begin
    if(clk'event and clk='1') then
      z(16) <= x(16) xor y(16);
      z(15) <= x(15) and y(15);
    end if;
  end process;
  addx <= x(14 downto 0);
  addy <= y(14 downto 0);

  u1: lpm_add_sub generic map(LPM_WIDTH=>15,LPM_PIPELINE=>1,LPM_DIRECTION=>"ADD")
       port map(dataa=>addx,datab=>addy,clock=>clk,result=>addz);
  z(14 downto 0) <= addz;

end rtl;

library ieee;
use ieee.std_logic_1164.all;

entity pg_pdelay_12 is
  generic (PG_WIDTH: integer);
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);
	   clk : in std_logic);
end pg_pdelay_12;

architecture rtl of pg_pdelay_12 is

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

begin

  x0 <= x;

  process(clk) begin
    if(clk'event and clk='1') then
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

  y <= x12;

end rtl;

library ieee;
use ieee.std_logic_1164.all;

entity pg_log_div_17_1 is
  port( x,y : in std_logic_vector(16 downto 0);
          z : out std_logic_vector(16 downto 0);
        clk : in std_logic);
end pg_log_div_17_1;

architecture rtl of pg_log_div_17_1 is

  component lpm_add_sub
    generic (LPM_WIDTH: integer;
             LPM_PIPELINE: integer;
             LPM_DIRECTION: string);
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);
          clock: in std_logic;
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));
  end component;

 signal addx,addy,addz: std_logic_vector(14 downto 0); 

begin

  process(clk) begin
    if(clk'event and clk='1') then
      z(16) <= x(16) xor y(16);
      z(15) <= x(15) and y(15);
    end if;
  end process;
  addx <= x(14 downto 0);
  addy <= y(14 downto 0);

  u1: lpm_add_sub generic map(LPM_WIDTH=>15,LPM_PIPELINE=>1,LPM_DIRECTION=>"SUB")
       port map(dataa=>addx,datab=>addy,clock=>clk,result=>addz);
  z(14 downto 0) <= addz;

end rtl;
-- ***************************************************************
-- * PGPG LOGARITHMIC TO FIXED-POINT FORMAT CONVERTER            *
-- *  AUTHOR: Tsuyoshi Hamada                                    *
-- *  VERSION: 2.00                                              *
-- *  LAST MODIFIED AT Tue Jun 03 22:52:01 JST 2003              *
-- ***************************************************************
library ieee;
use ieee.std_logic_1164.all;

entity pg_conv_ltof_17_8_57_2 is
  port(logdata : in std_logic_vector(16 downto 0);
       fixdata : out std_logic_vector(56 downto 0);
       clk : in std_logic);
end pg_conv_ltof_17_8_57_2;

architecture rtl of pg_conv_ltof_17_8_57_2 is

  component table
    generic (IN_WIDTH: integer;
             OUT_WIDTH: integer;
             TABLE_FILE: string);
    port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);   
         outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);
         clk : in std_logic);
  end component;

  component shift_ltof_9_56
    port( indata : in std_logic_vector(8 downto 0);
          control : in std_logic_vector(6 downto 0);
          clk : in std_logic;
          outdata : out std_logic_vector(55 downto 0)); 
  end component;

  signal frac1 : std_logic_vector(7 downto 0);
  signal frac2 : std_logic_vector(8 downto 0);
  signal sign1 : std_logic;
  signal sign2 : std_logic;
  signal sign3 : std_logic;
  signal sign4 : std_logic;
  signal sign5 : std_logic;
  signal sign6 : std_logic;
  signal nz1 : std_logic;
  signal exp1 : std_logic_vector(6 downto 0);
  signal fix1 : std_logic_vector(55 downto 0);

begin

  process(clk) begin
    if(clk'event and clk='1') then
      sign1 <= logdata(16);
      nz1 <= logdata(15);
      exp1 <= logdata(14 downto 8);
    end if;
  end process;

  u1: table generic map (IN_WIDTH=>8,OUT_WIDTH=>8,TABLE_FILE=>"ltf0.mif")
          port map(indata=>logdata(7 downto 0),outdata=>frac1,clk=>clk);

  with nz1 select
    frac2 <= '1' & frac1 when '1',
             "000000000" when others;

  -------------------------------------------------------------------
  -- PIPELINE 3,4,5,6,7 STAGES
  u3: shift_ltof_9_56
            port map (indata=>frac2,control=>exp1,outdata=>fix1,clk=>clk);
  sign6 <= sign5;
  sign5 <= sign4;
  sign4 <= sign3;
  sign3 <= sign2;
  sign2 <= sign1;
  -------------------------------------------------------------------

  process(clk) begin                
    if(clk'event and clk='1') then  
      fixdata(56) <= sign5;         
      fixdata(55 downto 0) <= fix1; 
    end if;                         
  end process;                      

end rtl;
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity shift_ltof_9_56 is                        
  port( indata : in std_logic_vector(8 downto 0);       
        control : in std_logic_vector(6 downto 0);     
        clk : in std_logic;                                         
        outdata : out std_logic_vector(55 downto 0));   
end shift_ltof_9_56;                             
                                                                    
architecture rtl of shift_ltof_9_56 is
                                                                    
  component lpm_clshift                                             
    generic (LPM_WIDTH : POSITIVE;                                  
             LPM_WIDTHDIST : POSITIVE);                             
    port(data : in std_logic_vector(LPM_WIDTH-1 downto 0);          
         distance : in std_logic_vector(LPM_WIDTHDIST-1 downto 0);  
         result : out std_logic_vector(LPM_WIDTH-1 downto 0));      
  end component;                                                    
                                                                    
  signal d0,o0 : std_logic_vector(71 downto 0);          
                                                                    
begin                                                               
                                                                    
  d0 <= "000000000000000000000000000000000000000000000000000000000000000" & indata;                                        
  u1: lpm_clshift generic map (LPM_WIDTH=>72,LPM_WIDTHDIST=>6)
          port map (data=>d0,distance=>control(5 downto 0),result=>o0);
  outdata <= o0(63 downto 8);                
                                                                    
end rtl;                                                            

-- pg_fix_accum
-- Pipelined, Virtual Multiple Pipelined Fixed-Point Accumulator for Programmable GRAPE
-- Author: Tsuyoshi Hamada
-- Last Modified at May 9 13:33:33
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity pg_fix_accum_57_64_2 is
  port (fdata: in std_logic_vector(56 downto 0);
        sdata0: out std_logic_vector(63 downto 0);
        sdata1: out std_logic_vector(63 downto 0);
        run : in std_logic;
        clk : in std_logic);
end pg_fix_accum_57_64_2;

architecture rtl of pg_fix_accum_57_64_2 is

  component fix_accum_reg_2
    generic (WIDTH: integer);
    port (indata: in std_logic_vector(WIDTH-1 downto 0);
          count : in std_logic;
          countd : out std_logic;
          cin : in std_logic;
          run : in std_logic;
          addsub : in std_logic;
          addsubd : out std_logic;
          rund : out std_logic;
          cout : out std_logic;
          reg0 : out std_logic_vector(WIDTH-1 downto 0);
          reg1 : out std_logic_vector(WIDTH-1 downto 0);
          clk : in std_logic);
  end component;

  component fix_accum_reg_last_2
    generic (WIDTH: integer);
    port (indata: in std_logic_vector(WIDTH-1 downto 0);
          count : in std_logic;
          cin : in std_logic;
          run : in std_logic;
          addsub : in std_logic;
--        countd : out std_logic_vector(0 downto 0);
--        addsubd : out std_logic;
--        rund : out std_logic;
--        cout : out std_logic;
          reg0 : out std_logic_vector(WIDTH-1 downto 0);
          reg1 : out std_logic_vector(WIDTH-1 downto 0);
          clk : in std_logic);
  end component;

  signal fx: std_logic_vector(63 downto 0);
  signal fx_0_0: std_logic_vector(31 downto 0);
  signal fx_1_0: std_logic_vector(31 downto 0);
  signal fx_1_1: std_logic_vector(31 downto 0);
  signal rsig : std_logic_vector(1 downto 0);
  signal addsub : std_logic_vector(1 downto 0);
  signal carry : std_logic_vector(1 downto 0);
  signal count0 : std_logic;
  signal count1 : std_logic;

begin

  fx <= "00000000" & fdata(55 downto 0);

  fx_0_0 <= fx(31 downto 0);
  fx_1_0 <= fx(63 downto 32);
  addsub(0) <= not fdata(56);
  carry(0) <= fdata(56);
  rsig(0)  <= run;


  process(clk) begin                        
    if(clk'event and clk='1') then          
      if(rsig(1) = '1') then                
        if(count0 = '1') then               
          count0 <= '0';                    
        else                                
          count0 <= '1';                    
        end if;                             
      else                                  
        if(rsig(0) = '1') then              
          count0 <= '0';                    
        end if;                             
      end if;                               
    end if;                                 
  end process;                              

  process(clk) begin              
    if(clk'event and clk='1') then
      fx_1_1 <= fx_1_0;       
    end if;                       
  end process;                    

  u0: fix_accum_reg_2                 
    generic map(WIDTH=>32)                
    port map(indata=>fx_0_0,            
             count=>count0,            
             cin=>carry(0),              
             run=>rsig(0),               
             addsub=>addsub(0),          
             countd=>count1,         
             addsubd=>addsub(1),       
             rund=>rsig(1),            
             cout=>carry(1),           
             reg0=>sdata0(31 downto 0),
             reg1=>sdata1(31 downto 0),
             clk=>clk);                   
                                          
  u1: fix_accum_reg_last_2            
    generic map(WIDTH=>32)                
    port map(indata=>fx_1_1,            
             count=>count1,            
             cin=>carry(1),              
             run=>rsig(1),               
             addsub=>addsub(1),          
--           countd=>count2,           
--           addsubd=>addsub(2),       
--           rund=>rsig(2),            
--           cout=>carry(2),           
             reg0=>sdata0(63 downto 32),
             reg1=>sdata1(63 downto 32),
             clk=>clk);                   
                                          
end rtl;                                                            
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
use ieee.std_logic_unsigned.all;                                    
                                                                    
entity fix_accum_reg_2 is
  generic(WIDTH: integer := 28);                                    
  port(indata : in std_logic_vector(WIDTH-1 downto 0);              
       count : in std_logic;
       cin : in std_logic;     
       run : in std_logic;     
       addsub : in std_logic;  
       countd : out std_logic;
       addsubd : out std_logic;
       rund : out std_logic;   
       cout : out std_logic;   
       reg0 : out std_logic_vector(WIDTH-1 downto 0);
       reg1 : out std_logic_vector(WIDTH-1 downto 0);
       clk : in std_logic);    
end fix_accum_reg_2;
                                                                    
architecture rtl of fix_accum_reg_2 is                                           
                                                                    
  component lpm_add_sub                                             
    generic (LPM_WIDTH: integer);                                   
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);   
          cin : in std_logic;                                       
          add_sub : in std_logic;                                   
          cout : out std_logic;                                     
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));      
  end component;                                                    
                                                                    
signal sx : std_logic_vector(WIDTH-1 downto 0);                     
signal zero : std_logic_vector(WIDTH-1 downto 0);                   
signal addout : std_logic_vector(WIDTH-1 downto 0);                 
signal run1 : std_logic;                                            
signal cout0 : std_logic;                                         
signal reg_vmp0 : std_logic_vector(WIDTH-1 downto 0);            
signal reg_vmp1 : std_logic_vector(WIDTH-1 downto 0);            

begin                                                               

  forgen1 : for i in 0 to WIDTH-1 generate
    zero(i) <= '0';                       
  end generate;                           

  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run1 = '1') then                                         
        if(count = '0') then                                     
          reg_vmp0 <= addout;                                    
        end if;                                                   
      else                                                        
        if(run = '1') then                                        
          reg_vmp0 <= zero;
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
  reg0 <= reg_vmp0;                                   

  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run1 = '1') then                                         
        if(count = '1') then                                     
          reg_vmp1 <= addout;                                    
        end if;                                                   
      else                                                        
        if(run = '1') then                                        
          reg_vmp1 <= zero;
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
  reg1 <= reg_vmp1;                                   

                                   
  with count select                
    sx <= reg_vmp1 when '1',       
          reg_vmp0 when others;    

  u1: lpm_add_sub generic map(LPM_WIDTH=>WIDTH)
      port map(dataa=>sx,                      
               datab=>indata,add_sub=>addsub,  
               cin=>cin,                       
               cout=>cout0,                    
               result=>addout);                

  process(clk) begin              
    if(clk'event and clk='1') then
      run1 <= run;                
    end if;                       
  end process;                    

  process(clk) begin              
    if(clk'event and clk='1') then
      addsubd <= addsub;          
      cout <= cout0;              
      countd <= count;            
    end if;                       
  end process;                    

  rund <= run1;

end rtl;                            
                                    
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
use ieee.std_logic_unsigned.all;                                    
                                                                    
entity fix_accum_reg_last_2 is
  generic(WIDTH: integer := 28);                                    
  port(indata : in std_logic_vector(WIDTH-1 downto 0);              
       count : in std_logic;
       cin : in std_logic;     
       run : in std_logic;     
       addsub : in std_logic;  
--     countd : out std_logic_vector(0 downto 0);
--     addsubd : out std_logic;
--     rund : out std_logic;   
--     cout : out std_logic;   
       reg0 : out std_logic_vector(WIDTH-1 downto 0);
       reg1 : out std_logic_vector(WIDTH-1 downto 0);
       clk : in std_logic);    
end fix_accum_reg_last_2;
                                                                    
architecture rtl of fix_accum_reg_last_2 is                                           
                                                                    
  component lpm_add_sub                                             
    generic (LPM_WIDTH: integer);                                   
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);   
          cin : in std_logic;                                       
          add_sub : in std_logic;                                   
--          cout : out std_logic;                                     
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));      
  end component;                                                    
                                                                    
signal sx : std_logic_vector(WIDTH-1 downto 0);                     
signal zero : std_logic_vector(WIDTH-1 downto 0);                   
signal addout : std_logic_vector(WIDTH-1 downto 0);                 
signal run1 : std_logic;                                            
--signal cout0 : std_logic;                                       
signal reg_vmp0 : std_logic_vector(WIDTH-1 downto 0);            
signal reg_vmp1 : std_logic_vector(WIDTH-1 downto 0);            

begin                                                               

  forgen1 : for i in 0 to WIDTH-1 generate
    zero(i) <= '0';                       
  end generate;                           

  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run1 = '1') then                                         
        if(count = '0') then                                     
          reg_vmp0 <= addout;                                    
        end if;                                                   
      else                                                        
        if(run = '1') then                                        
          reg_vmp0 <= zero;
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
  reg0 <= reg_vmp0;                                   

  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run1 = '1') then                                         
        if(count = '1') then                                     
          reg_vmp1 <= addout;                                    
        end if;                                                   
      else                                                        
        if(run = '1') then                                        
          reg_vmp1 <= zero;
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
  reg1 <= reg_vmp1;                                   

                                   
  with count select                
    sx <= reg_vmp1 when '1',       
          reg_vmp0 when others;    

  u1: lpm_add_sub generic map(LPM_WIDTH=>WIDTH)
      port map(dataa=>sx,                      
               datab=>indata,add_sub=>addsub,  
               cin=>cin,                       
--               cout=>cout0,                    
               result=>addout);                

  process(clk) begin              
    if(clk'event and clk='1') then
      run1 <= run;                
    end if;                       
  end process;                    

--process(clk) begin              
--  if(clk'event and clk='1') then
--    addsubd <= addsub;          
--    cout <= cout0;              
--    countd <= count;            
--  end if;                       
--end process;                    

--rund <= run1;

end rtl;                            
                                    

--+-------------------------------+
--| PG_LCELL/PG_LCELL_ARI         |
--| Multidevice Logic Cell Module |.GKey:hYj8zgUkjgdkfhB3ozXM
--| 2003/10/31                    |
--|            by Tsuyoshi Hamada |
--+-------------------------------+
--+-------------+----------+
--| x3,x2,x1,x0 |    z     |
--+-------------+----------+
--|  0, 0, 0, 0 | MASK(0)  |
--|  0, 0, 0, 1 | MASK(1)  |
--|  0, 0, 1, 0 | MASK(2)  |
--|  0, 0, 1, 1 | MASK(3)  |
--|  0, 1, 0, 0 | MASK(4)  |
--|  0, 1, 0, 1 | MASK(5)  |
--|  .......... | .......  |
--|  1, 1, 1, 1 | MASK(16) |
--+-------------+----------+
library ieee;
use ieee.std_logic_1164.all;
library wysiwyg;
use wysiwyg.stratix_components.all;

entity pg_lcell is
  generic(MASK: string  := "ffff";
            FF: integer := 0);
    port(x : in std_logic_vector(3 downto 0);
         z : out std_logic;
         clk : in std_logic);
end pg_lcell;

architecture rtl of pg_lcell is

component stratix_lcell                         
  generic                                       
  (                                             
    operation_mode : string := "normal";      
    synch_mode     : string := "off";         
    register_cascade_mode : string := "off";  
    sum_lutc_input : string := "datac";       
    lut_mask       : string := "0000";        
    power_up       : string := "low";         
    cin0_used      : string := "false";       
    cin1_used      : string := "false";       
    cin_used       : string := "false";       
    output_mode    : string := "comb_and_reg" 
  );                                            
  port (clk: in std_logic := '0';        
    dataa  : in std_logic := '1';        
    datab  : in std_logic := '1';        
    datac  : in std_logic := '1';        
    datad  : in std_logic := '1';        
--  aclr   : in std_logic := '0';        
--  aload  : in std_logic := '0';        
--  sclr   : in std_logic := '0';        
--  sload  : in std_logic := '0';        
    ena    : in std_logic := '1';        
--  cin    : in std_logic := '0';        
--  cin0   : in std_logic := '0';        
--  cin1   : in std_logic := '1';        
--  inverta   : in std_logic := '0';     
--  regcascin : in std_logic := '0';     
    combout   : out std_logic;           
    regout    : out std_logic);          
--  cout   : out std_logic;              
--  cout0  : out std_logic;              
--  cout1  : out std_logic);             
end component;                           
begin                                      
ifgen0: if (FF=0) generate
  u0: stratix_lcell generic map(lut_mask=>MASK)
      port map(dataa=>x(0),datab=>x(1),datac=>x(2),datad=>x(3),
               clk=>clk,combout=>z);
end generate;
ifgen1: if (FF>0) generate
  u0: stratix_lcell generic map(lut_mask=>MASK)
      port map(dataa=>x(0),datab=>x(1),datac=>x(2),datad=>x(3),
               clk=>clk,regout=>z);
end generate;
end rtl;

--+--------+----------------------+
--| ci,y,x | z       , co         |
--+--------+----------------------+
--|  0,0,0 | MASK(8) , MASK(0)    |
--|  0,0,1 | MASK(9) , MASK(1)    |
--|  0,1,0 | MASK(10), MASK(2)    |
--|  0,1,1 | MASK(11), MASK(3)    |
--|  1,0,0 | MASK(12), MASK(4)    |
--|  1,0,1 | MASK(13), MASK(5)    |
--|  1,1,0 | MASK(14), MASK(6)    |
--|  1,1,1 | MASK(15), MASK(7)    |
--+--------+----------------------+
library ieee;
use ieee.std_logic_1164.all;
library wysiwyg;
use wysiwyg.stratix_components.all;

entity pg_lcell_ari is
  generic(MASK: string  := "96E8";
            FF: integer := 0);
    port(x,y,ci : in std_logic;
         z,co : out std_logic;
         clk : in std_logic);
end pg_lcell_ari;

architecture rtl of pg_lcell_ari is

component stratix_lcell                         
   generic                                      
  (                                             
   operation_mode    : string := "arithmetic";
   synch_mode : string := "off";              
   register_cascade_mode   : string := "off"; 
-- sum_lutc_input : string := "datac";        
   lut_mask       : string := "96E8";         
   power_up       : string := "low";          
   cin0_used      : string := "false";        
   cin1_used      : string := "false";        
   cin_used       : string := "true";         
   output_mode    : string := "comb_and_reg"  
  );                                            
 port (clk : in std_logic := '0';    
   dataa   : in std_logic := '1';    
   datab   : in std_logic := '1';    
-- datac   : in std_logic := '1';    
-- datad   : in std_logic := '1';    
-- aclr    : in std_logic := '0';    
-- aload   : in std_logic := '0';    
-- sclr    : in std_logic := '0';    
-- sload   : in std_logic := '0';    
   ena     : in std_logic := '1';    
   cin     : in std_logic := '0';    
-- cin0    : in std_logic := '0';    
-- cin1    : in std_logic := '1';    
-- inverta : in std_logic := '0';    
-- regcascin : in std_logic := '0';  
   combout   : out std_logic;        
   regout    : out std_logic;        
   cout   : out std_logic);          
-- cout0  : out std_logic;           
-- cout1  : out std_logic);          
end component;                       
begin                                    
                                         
ifgen0: if (FF=0) generate
  u0: stratix_lcell generic map(lut_mask=>MASK) 
      port map(dataa=>x,datab=>y,cin=>ci,combout=>z,cout=>co,clk=>clk);
end generate;
ifgen1: if (FF>0) generate
  u0: stratix_lcell generic map(lut_mask=>MASK) 
      port map(dataa=>x,datab=>y,cin=>ci,regout=>z,cout=>co,clk=>clk);
end generate;

end rtl;
