library ieee;
use ieee.std_logic_1164.all;
LIBRARY altera_mf;
USE altera_mf.altera_mf_components.all;
LIBRARY altera;
USE altera.maxplus2.all;

entity pgfpga is 
  port(
-- for IO 
      e_clk,e_sysclk,e_rst: in std_logic;
      e_jpwe : in std_logic;
      e_pid: in std_logic_vector(9 downto 0);
      e_ipd : in std_logic_vector(35 downto 0);
      e_ipwe : in std_logic;
      e_fod: out std_logic_vector(35 downto 0);
      e_nd,e_vd,e_sts: out std_logic;
      e_wd : in std_logic;
      e_active : out std_logic;

-- for memory
      m_oe : out std_logic_vector(1 downto 0);
      m_we : out std_logic_vector(1 downto 0);
      m_al : out std_logic_vector(18 downto 0);
      m_ah : out std_logic_vector(18 downto 0);
      m_dl : inout std_logic_vector(35 downto 0);
      m_dh : inout std_logic_vector(35 downto 0);
      m_clk : out std_logic_vector(1 downto 0)
  );
end pgfpga;

architecture hierarchy of pgfpga is

  component ipw
    port( 
      i_ipwe: in std_logic;
      i_ipd : in std_logic_vector(35 downto 0);
      i_weca,i_wejp,i_wefo,i_wei: out std_logic;
      i_adri : out std_logic_vector(11 downto 0);
      i_adr : out std_logic_vector(3 downto 0);
      i_data: out std_logic_vector(31 downto 0);
      i_perr: out std_logic;
      clk,sysclk,rst : in std_logic );
  end component;

  component jpw
    port( 
      j_jpwe: in std_logic;
      j_jpd : in std_logic_vector(35 downto 0);
      j_physid : in std_logic_vector(9 downto 0);
      j_wejp: in std_logic;
      j_data: in std_logic_vector(19 downto 0);
      j_adr : in std_logic_vector(3 downto 0);
      j_mdl : out std_logic_vector(35 downto 0);
      j_mdh : out std_logic_vector(35 downto 0);
      j_mwe,j_moe : out std_logic_vector(1 downto 0);
      j_mal : out std_logic_vector(18 downto 0);
      j_mah : out std_logic_vector(18 downto 0);
      j_oel,j_oeh : out std_logic;
      j_clma : in std_logic_vector(18 downto 0);
      j_vcido : out std_logic_vector(9 downto 0);
      j_perr : out std_logic;
      clk,sysclk,rst : in std_logic);
  end component;

  component calc
    port(
      c_md : in std_logic_vector(71 downto 0);
      c_ma : out std_logic_vector(18 downto 0);
      c_weca: in std_logic;
      c_data: in std_logic_vector(19 downto 0);
      c_adr : in std_logic_vector(3 downto 0);
      c_jdata : out std_logic_vector(71 downto 0); 	        
      c_run : out std_logic;
      rst,clk : in std_logic);
  end component;

  component fo
    port(
      f_wefo : in std_logic;
      f_data: in std_logic_vector(31 downto 0);
      f_adr : in std_logic_vector(3 downto 0);
      f_pd : in std_logic_vector(31 downto 0);      
      f_pa : out std_logic_vector(11 downto 0);
      f_csts : in std_logic;
      f_fodata : out std_logic_vector(35 downto 0);
      f_sts,f_vd,f_nd : out std_logic;
      f_wd : in std_logic;
      f_active : out std_logic;
      f_vcid : in std_logic_vector(9 downto 0);
      rst,clk,sysclk : in std_logic );
  end component;

  component pg_pipe
    port(
      p_jdata: in std_logic_vector(71 downto 0);
      p_run : in std_logic;
      p_we : in std_logic;
      p_adri : in std_logic_vector(11 downto 0);      
      p_datai : in std_logic_vector(31 downto 0);
      p_adro : in std_logic_vector(11 downto 0);      
      p_datao : out std_logic_vector(31 downto 0);
      p_runret : out std_logic;
      rst,clk : in std_logic );
  end component;

 signal clk, iclk0, iclk1, iclk2, clk0, clk1:  std_logic;
 signal weca,wejp,wefo,wei: std_logic;
 signal adr: std_logic_vector(3 downto 0);
 signal adri: std_logic_vector(11 downto 0);
 signal data: std_logic_vector(31 downto 0);
 signal clma: std_logic_vector(18 downto 0);
 signal clsts: std_logic;
 signal jdata: std_logic_vector(71 downto 0);
 signal run: std_logic;
 signal padr: std_logic_vector(11 downto 0);
 signal pdata: std_logic_vector(31 downto 0);
 signal mdlh: std_logic_vector(71 downto 0);
 signal iperr,jperr: std_logic; 
 signal vcid: std_logic_vector(9 downto 0);

 signal mdoel,mdoeh: std_logic;
 signal m_dl0 : std_logic_vector(35 downto 0);
 signal m_dh0 : std_logic_vector(35 downto 0);

 signal sysclk0,sysclk1:  std_logic; 

begin

--  upll: altclklock generic map(inclock_period => 58823,
--                              clock0_boost => 1,clock1_boost => 2)
--                  port map(inclock=>e_clk, clock0=>clk, clock1=>iclk0);
 
--  process(iclk0) begin
--    if(iclk0'event and iclk0='1') then 
--      iclk1 <= not iclk1;
--    end if;
--  end process;

--  m_clk(1) <= iclk1;
--  m_clk(0) <= iclk1;

--  ulcell0: lcell PORT MAP(a_in=>e_sysclk, a_out=>sysclk0);
--  ulcell1: lcell PORT MAP(a_in=>sysclk0, a_out=>sysclk1);

  clk <= e_clk;
  m_clk(1) <= e_sysclk;
  m_clk(0) <= e_sysclk;

  uipw: ipw PORT MAP(i_ipwe=>e_ipwe,i_ipd=>e_ipd,
		   i_weca=>weca, i_wejp=>wejp, i_wefo=>wefo, i_wei=>wei, 
		   i_adr=>adr, i_adri=>adri, i_data=>data, i_perr=>iperr,
                   clk=>clk,sysclk=>e_sysclk,rst=>e_rst);

  ujpw: jpw PORT MAP(j_jpwe=>e_jpwe, j_jpd=>e_ipd, j_physid=>e_pid, 
                    j_wejp=>wejp, j_data=>data(19 downto 0), j_adr=>adr(3 downto 0), 
	            j_mdl=>m_dl0, j_mdh=>m_dh0, j_mwe=>m_we, j_moe=>m_oe,
                    j_oel=>mdoel, j_oeh=>mdoeh, 
	            j_mal=>m_al, j_mah=>m_ah,j_clma=>clma, j_perr=>jperr,
                    j_vcido=>vcid, clk=>clk,sysclk=>e_sysclk,rst=>e_rst);

   
  with mdoel select
    m_dl <= m_dl0 when '1',
            "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ" when others;

  with mdoeh select
    m_dh <= m_dh0 when '1',
            "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ" when others;

  mdlh <= m_dh & m_dl;
  ucalc: calc PORT MAP(c_md=>mdlh, c_ma=>clma, 
                     c_weca=>weca, c_data=>data(19 downto 0), c_adr=>adr(3 downto 0), 
	             c_jdata=>jdata, c_run=>run, 
                     clk=>clk,rst=>e_rst);

  ufo: fo PORT MAP(f_wefo=>wefo,f_data=>data, f_adr=>adr(3 downto 0),
                   f_pd=>pdata, f_pa=>padr, f_csts=>clsts,
		   f_fodata=>e_fod, f_sts=>e_sts,
	           f_vd=>e_vd, f_nd=>e_nd, f_wd=>e_wd, 
	           f_active=>e_active, f_vcid=>vcid,
                   rst=>e_rst,clk=>clk,sysclk=>e_sysclk);


  upipeline: pg_pipe PORT MAP(p_jdata=>jdata, p_run=>run,
	                p_we=>wei,p_adri=>adri,p_datai=>data,
	                p_adro=>padr,p_datao=>pdata, p_runret=>clsts,
                        rst=>e_rst,clk=>clk);
	                  
end hierarchy;
  

library ieee;
use ieee.std_logic_1164.all;

entity parity_check is
port(
   data: in std_logic_vector(35 downto 0);
   clk,rst,vd : in std_logic;
   perr: out std_logic
);
end parity_check;

architecture RTL of parity_check is
  signal perr0 : std_logic;
begin

  process(clk)
    variable tmp: std_logic_vector(3 downto 0);  
  begin
    if(clk'event and clk='1') then 
      if(rst = '1') then 
        perr0 <= '0';
      else
	if(vd = '1') then 
          tmp := "0000";
          for i in 0 to 7 loop
            tmp(0) := tmp(0) xor data(i);
            tmp(1) := tmp(1) xor data(i+8);
            tmp(2) := tmp(2) xor data(i+16);
            tmp(3) := tmp(3) xor data(i+24);
          end loop;
          if((tmp(0) /= data(32)) or (tmp(1) /= data(33)) 
            or (tmp(2) /= data(34)) or (tmp(3) /= data(35))) then 
            perr0 <= perr0 or '1';
          end if;
        end if;
      end if;
    end if;
  end process;
  perr <= perr0;

end RTL;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity ipw is
    port( 
      i_ipwe: in std_logic;
      i_ipd : in std_logic_vector(35 downto 0);
      i_weca,i_wejp,i_wefo,i_wei: out std_logic;
      i_adri : out std_logic_vector(11 downto 0);
      i_adr : out std_logic_vector(3 downto 0);
      i_data: out std_logic_vector(31 downto 0);
      i_perr: out std_logic;
      clk,sysclk,rst : in std_logic );
end ipw;

architecture rtl of ipw is

  component parity_check
  port(
    data: in std_logic_vector(35 downto 0);
    clk,rst,vd: in std_logic;
    perr: out std_logic  );
  end component;

  signal ipd0,ipd1 : std_logic_vector(35 downto 0);
  signal sysclk0,sysclk1 : std_logic;
  signal ipwe0,ipwe1 : std_logic;
  signal vd : std_logic;
  signal cefp0,weiprg0,wejp0,wefo0,wecalc0 : std_logic;
  signal diprg: std_logic_vector(31 downto 0);
  signal aiprg : std_logic_vector(5 downto 0);
  signal com_dc : std_logic_vector(1 downto 0);
  signal word_dc : std_logic_vector(10 downto 0);
  signal adrdec : std_logic_vector(14 downto 0);
  signal radr_uc : std_logic_vector(3 downto 0);
  signal fadr_uc : std_logic_vector(11 downto 0);

begin

-- data latch at input port

  process(clk) begin
    if(clk'event and clk='1') then
      sysclk1 <= sysclk0;
      sysclk0 <= sysclk;
      ipwe1 <= ipwe0;
      ipwe0 <= i_ipwe;
      ipd1 <= ipd0;
      ipd0 <= i_ipd;	
    end if;
  end process;

-- check write enable

  process(sysclk0,sysclk1,ipwe1) begin
--    if(sysclk0='1' and sysclk1='0' and ipwe1='0') then  
    if(ipwe1='0') then  
      vd <= '1';
    else 
      vd <= '0';
    end if;
  end process;

-- command counter

  process(clk) begin
    if(clk'event and clk='1') then 
      if(rst='1') then 
        com_dc <= "00";
      elsif(vd='1') then
        if(com_dc = "00" and word_dc = "00000000000") then 
          com_dc <= "10";
	elsif(com_dc /=  "00") then 
          com_dc <= com_dc - "01";
        end if;
      end if;
    end if;
  end process;

-- word counter

  process(clk) begin
    if(clk'event and clk='1') then
      if(rst = '1') then 
        word_dc <= "00000000000";
      elsif(vd = '1') then 
        if(com_dc = "10") then 
           word_dc <= ipd1(10 downto 0);
        elsif(word_dc /= "00000000000") then
           word_dc <= word_dc - "00000000001";
        end if;
      end if;
    end if;
  end process;

-- address latch and decoder

  process(clk) begin
    if(clk'event and clk='1') then
      if(vd = '1') then 
        if(com_dc = "00" and word_dc = "00000000000") then 
          adrdec <= ipd1(14 downto 0);
        end if;
      end if;
    end if;
  end process;

  with adrdec(14 downto 12) select
    cefp0 <= '1' when "000",
             '0' when others;

  with adrdec(14 downto 12) select
    weiprg0 <= '1' when "001",
               '0' when others;

  with adrdec(14 downto 12) select
    wejp0 <= '1' when "010",
             '0' when others;

  with adrdec(14 downto 12) select
    wefo0 <= '1' when "011",
             '0' when others;

  with adrdec(14 downto 12) select
    wecalc0 <= '1' when "100",
               '0' when others;

-- register address counter 

  process(clk) begin   
    if(clk'event and clk='1') then
      if(vd ='1') then 
        if(com_dc = "00" and word_dc = "00000000000") then
          radr_uc <= ipd1(3 downto 0);
        elsif(word_dc /= "00000000000" and cefp0 = '0') then 
          radr_uc <=  radr_uc + "0001";
	end if;
      end if;
    end if;
  end process;

-- pipeline address counter 

  process(clk) begin   
    if(clk'event and clk='1') then
      if(vd ='1') then 
        if(com_dc = "00" and word_dc = "00000000000") then
          fadr_uc <= ipd1(11 downto 0);
        elsif(word_dc /= "00000000000" and cefp0 = '1') then 
          fadr_uc <=  fadr_uc + "000000000001";
	end if;
      end if;
    end if;
  end process;

-- parity checker

   pcheck: parity_check PORT MAP(data=>ipd1,clk=>clk,rst=>rst,vd=>vd,
				perr=>i_perr);

-- write enable signal

  process(clk) begin
    if(clk'event and clk='1') then 
      if(vd='1' and word_dc /= "00000000000") then
	i_adr <= radr_uc;
        i_data <= ipd1(31 downto 0);
        i_adri <= fadr_uc;
        i_wejp <= wejp0;
        i_weca <= wecalc0;
        i_wefo <= wefo0;
        i_wei <= cefp0;
      else
        i_wejp <= '0';
        i_wefo <= '0';
        i_weca <= '0';
        i_wei <= '0';
      end if;
    end if;
  end process;

-- write to command register in ipw

--  diprg <= ipd1(31 downto 0); 
--  aiprg <= radr_uc;
  process(clk) begin
    if(clk'event and clk='1') then 
      if(weiprg0='1') then 
      end if;	
    end if;
  end process;
     
end RTL;
  



library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity jpw is
    port( 
      j_jpwe: in std_logic;
      j_jpd : in std_logic_vector(35 downto 0);
      j_physid : in std_logic_vector(9 downto 0);
      j_wejp: in std_logic;
      j_data: in std_logic_vector(19 downto 0);
      j_adr : in std_logic_vector(3 downto 0);
      j_mdl : out std_logic_vector(35 downto 0);
      j_mdh : out std_logic_vector(35 downto 0);
      j_mwe,j_moe : out std_logic_vector(1 downto 0);
      j_mal : out std_logic_vector(18 downto 0);
      j_mah : out std_logic_vector(18 downto 0);
      j_oel,j_oeh : out std_logic;
      j_clma : in std_logic_vector(18 downto 0);
      j_vcido : out std_logic_vector(9 downto 0);
      j_perr : out std_logic;
      clk,sysclk,rst : in std_logic);
end jpw;

architecture RTL of jpw is

  component parity_check
  port(
    data: in std_logic_vector(35 downto 0);
    clk,rst,vd: in std_logic;
    perr: out std_logic  );
  end component;

  signal sysclk0,sysclk1,jpwe0,jpwe1: std_logic;
  signal jpd0,jpd1: std_logic_vector(35 downto 0);
  signal vd : std_logic;
  signal com_dc: std_logic_vector(1 downto 0);
  signal word_dc: std_logic_vector(4 downto 0);
  signal word_uc: std_logic_vector(3 downto 0);
  signal selid : std_logic;
  signal reg1: std_logic_vector(31 downto 0);
  signal memadr0,memadr1,memadr2: std_logic_vector(18 downto 0);
  signal weregl,weregh: std_logic;
  signal nd : std_logic_vector(4 downto 0);
  signal vcid : std_logic_vector(9 downto 0);

begin

-- data latch at input port

  process(clk) begin
    if(clk'event and clk='1') then
      sysclk1 <= sysclk0;
      sysclk0 <= sysclk;
      jpwe1 <= jpwe0;
      jpwe0 <= j_jpwe;
      jpd1 <= jpd0;
      jpd0 <= j_jpd;	
    end if;
  end process;

-- check write enable (asyn)

  process(sysclk0,sysclk1,jpwe1) begin
--    if(sysclk0='1' and sysclk1='0' and jpwe1='0') then  
    if(jpwe1='0') then  
      vd <= '1';
    else 
      vd <= '0';
    end if;
  end process;

-- parity checker

  pcheck: parity_check PORT MAP(data=>jpd1,clk=>clk,rst=>rst,perr=>j_perr,vd=>vd);

-- command counter

  process(clk) begin
    if(clk'event and clk='1') then 
      if(rst = '1') then 
         com_dc <= "00"; 
      elsif(vd='1') then
        if(com_dc = "00" and word_dc = "00000") then
          com_dc <= "10";
	elsif(com_dc /= "00") then 
          com_dc <= com_dc -"01";
        end if;
      end if;
    end if;
  end process;

-- word counter 

  process(clk) begin
    if(clk'event and clk='1') then
      if(rst = '1') then 
        word_dc <= "00000";
      elsif(vd = '1') then 
        if(com_dc="10") then 
          word_dc <= nd;
          word_uc <= "0000";
        elsif(word_dc /= "00000") then
          word_dc <= word_dc - "00001";
          word_uc <= word_uc + "0001";
        end if;
      end if;
    end if;
  end process;

-- check ID

  process(clk) begin
    if(clk'event and clk='1') then 
      if(vd='1') then 
        if(com_dc = "00" and word_dc="00000") then
          if((jpd1(9 downto 0) and jpd1(19 downto 10)) = (vcid and jpd1(19 downto 10))) then 
            selid <= '1';
          else
            selid <= '0';
	  end if;
        end if;
      end if;
    end if;
  end process;

-- write to register 0 

  process(clk) begin
    if(clk'event and clk='1') then 
      if(vd='1') then 
        reg1 <= jpd1(31 downto 0);
      end if;
    end if;
  end process;

-- memory address latch and counter
 
  process(clk) begin   
    if(clk'event and clk='1') then
      if(vd ='1') then 
        if(com_dc = "10") then
          memadr0 <= jpd1(18 downto 0);
	else
          if(word_uc(0) = '1') then 
            memadr0 <= memadr0 + "000000000000000001";
          end if;
        end if;
      end if;
    end if;
  end process;

  process(clk) begin   
    if(clk'event and clk='1') then
      memadr1 <= memadr0;
--      memadr2 <= memadr1;
    end if;
  end process;

-- memory control timing signal

  process(clk) begin   
    if(clk'event and clk='1') then
      if(word_dc /= "00000" and selid = '1' and word_uc(0) = '0') then 
        weregl <= '1';
      else 
	weregl <= '0';
      end if;	
    end if;
  end process;

  process(clk) begin   
    if(clk'event and clk='1') then
      if(word_dc /= "00000" and selid = '1' and word_uc(0) = '1') then 
        weregh <= '1';
      else 
	weregh <= '0';
      end if;	
    end if;
  end process;

-- mema signal

  process(clk) begin   
    if(clk'event and clk='1') then
      if(weregl = '1') then 
	j_mal <= memadr1;
      else 
	j_mal <= j_clma;
      end if;	
    end if;
  end process;

  process(clk) begin   
    if(clk'event and clk='1') then
      if(weregh = '1') then 
--	j_mah <= memadr2;
	j_mah <= memadr1;
      else
	j_mah <= j_clma;
      end if;	
    end if;
  end process;

-- memd signal


  process(clk) begin   
    if(clk'event and clk='1') then
      j_mdl <= "0000" & reg1;
      j_mdh <= "0000" & reg1;
      j_oel <= weregl;	
      j_oeh <= weregh;	
    end if;
  end process;

--  process(clk) begin   
--    if(clk'event and clk='1') then
--      if(weregl = '1') then 	
--        j_mdl <= "0000" & reg1;
--      else
--        j_mdl <= "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ";
--      end if;	
--    end if;
--  end process;
--
--  process(clk) begin   
--    if(clk'event and clk='1') then
--      if(weregh = '1') then 	
--        j_mdh <= "0000" & reg1;
--      else
--        j_mdh <= "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ";
--      end if;	
--    end if;
--  end process;

-- memwe signal

  process(clk) begin   
    if(clk'event and clk='1') then
      j_mwe(0) <= not weregl;
      j_mwe(1) <= not weregh;
    end if;
  end process;

-- memoe signal

  process(clk) begin   
    if(clk'event and clk='1') then
      j_moe(0) <= weregl;
      j_moe(1) <= weregh;
    end if;
  end process;

-- write to command register in jpw

  process(clk) begin
    if(clk'event and clk='1') then 
      if(j_wejp='1') then 
        if(j_adr(3 downto 0) = "0000") then 
          nd <= j_data(4 downto 0);
        elsif(j_adr(3 downto 0)= "0001") then            
	  if(j_data(19 downto 10)=j_physid) then 
            vcid <= j_data(9 downto 0);
 	  end if;
        end if; 
      end if;	
    end if;
  end process;
  j_vcido <= vcid;
     
end RTL;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity calc is
    port(
      c_md : in std_logic_vector(71 downto 0);
      c_ma : out std_logic_vector(18 downto 0);
      c_weca: in std_logic;
      c_data: in std_logic_vector(19 downto 0);
      c_adr : in std_logic_vector(3 downto 0);
      c_jdata : out std_logic_vector(71 downto 0);             
      c_run : out std_logic;
      rst,clk : in std_logic);
end calc;

architecture RTL of calc is

  signal mema_dc: std_logic_vector(19 downto 0);
  signal adrvd00: std_logic;
  signal adrvd: std_logic_vector(5 downto 0);
  signal n: std_logic_vector(19 downto 0);
  signal start: std_logic;

begin

-- generate memory address 

  process(clk) begin
    if(clk'event and clk='1') then 
      if(rst = '1') then 
        mema_dc <= "00000000000000000000";
      else
        if(start='1') then 
          mema_dc <= n ;
        elsif(mema_dc /= "00000000000000000000") then 
          mema_dc <= mema_dc - "00000000000000000001";
        end if;	
      end if;
    end if;
  end process;

  c_ma <= mema_dc(18 downto 0);

-- address and data valid signal

  process(clk) begin
    if(clk'event and clk='1') then 
      if(rst = '1') then
        adrvd00 <= '0';
      elsif(start = '1') then 
	adrvd00 <= '1';
      elsif(mema_dc = "0000000000000000000001") then 
	adrvd00 <= '0';
      end if;
    end if;
  end process;

  process(clk) begin
    if(clk'event and clk='1') then 
      if(rst = '1') then  
        adrvd <= "000000";
      else
        adrvd(1) <= adrvd00;        
        adrvd(2) <= adrvd(1);        
        adrvd(3) <= adrvd(2);        
        adrvd(4) <= adrvd(3);        
        adrvd(5) <= adrvd(4);        
      end if; 
    end if;
  end process;

  c_run <= adrvd(4);

-- data from memory

  process(clk) begin
    if(clk'event and clk='1') then 
      c_jdata <= c_md(71 downto 68) & c_md(35 downto 32) & c_md(67 downto 36) & c_md(31 downto 0);
    end if;
  end process;

-- write to command register in calc

  process(clk) begin
    if(clk'event and clk='1') then 
      if(c_weca='1') then 
        if(c_adr = "0000") then 
	  n <= c_data;
        end if;
      end if;	
    end if;
  end process;
     
  process(clk) begin
    if(clk'event and clk='1') then 
      if(rst = '1') then 
        start <=  '0';
      elsif(c_weca='1' and c_adr = "0000") then 
        start <=  '1';
      else
	start <= '0';
      end if;	
    end if;
  end process;

end RTL;



  



library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity fo is
    port(
      f_wefo : in std_logic;
      f_data: in std_logic_vector(31 downto 0);
      f_adr : in std_logic_vector(3 downto 0);
      f_pd : in std_logic_vector(31 downto 0);      
      f_pa : out std_logic_vector(11 downto 0);
      f_csts : in std_logic;
      f_fodata : out std_logic_vector(35 downto 0);
      f_sts,f_vd,f_nd : out std_logic;
      f_wd : in std_logic;
      f_active : out std_logic;
      f_vcid : in std_logic_vector(9 downto 0);	
      rst,clk,sysclk : in std_logic );
end fo;

architecture RTL of fo is

  signal sysclk0,sysclk1 : std_logic;
  signal scphase : std_logic;
  signal calcsts1 : std_logic;
  signal plstart_flag,rdstart_flag: std_logic;
  signal select_code: std_logic_vector(1 downto 0);
  signal al_dc,al_uc: std_logic_vector(3 downto 0);
  signal ah_dc,ah_uc: std_logic_vector(7 downto 0);
  signal address_pl : std_logic_vector(11 downto 0);
  signal vdflag_pl: std_logic;
  signal random_dc : std_logic_vector(1 downto 0);
  signal random_uc : std_logic_vector(11 downto 0);
  signal address_rd : std_logic_vector(11 downto 0);
  signal fodata0: std_logic_vector(31 downto 0);
  signal vdflag_rd: std_logic;
  signal vd0,vd1: std_logic;
  signal com0,com1: std_logic_vector(31 downto 0);
  signal ni : std_logic_vector(5 downto 0);
  signal ndata : std_logic_vector(3 downto 0);
  signal comstart: std_logic;

begin

  f_active <= '1';

-- make phase and wd from sysclk 

  process(clk) begin
    if(clk'event and clk='1') then
      sysclk1 <= sysclk0;
      sysclk0 <= sysclk;
    end if;
  end process;

  scphase <= '1';
	 
-- triger logic

  process(clk) begin
    if(clk'event and clk='1') then
      if(scphase = '1') then 
	calcsts1 <= f_csts;
      end if;	
    end if;
  end process;

  process(clk) begin
    if(clk'event and clk='1') then
      if(scphase = '1') then 
        if((f_csts = '0') and (calcsts1 = '1')) then 
          plstart_flag <= '1';
        else 
          plstart_flag <= '0';	
	end if;
      end if;
    end if;
  end process;

  process(clk) begin
    if(clk'event and clk='1') then
      if(scphase = '1') then 
        if((comstart = '1') and (com0(31 downto 30) = "00") 
 	    and ((com0(9 downto 0) and com0(19 downto 10)) 
		= (f_vcid and com0(19 downto 10))) and (ah_dc = "000000")) then 	
          rdstart_flag <= '1';
        else
          rdstart_flag <= '0';
	end if;
      end if;
    end if;
  end process;

  process(clk) begin
    if(clk'event and clk='1') then
      if(plstart_flag = '1') then 
        select_code <= "00";
      elsif(rdstart_flag = '1') then 
        select_code <= "10";
      end if;	
    end if;
  end process;

-- pipeline read counter

  process(clk) begin
    if(clk'event and clk='1') then
      if(scphase = '1') then 
        if(plstart_flag = '1' or (al_dc = "0001" and ah_dc /= "00000001")) then 
	  al_dc <= ndata;
	  al_uc <= "0000";
        elsif(al_dc /= "0000") then 
	  al_dc <= al_dc - "0001";
	  al_uc <= al_uc + "0001";
	end if;
      end if;
    end if;
  end process;

  process(clk) begin
    if(clk'event and clk='1') then
      if(rst='1') then  
	  ah_dc <= "00000000";
      elsif(scphase = '1') then 
        if(plstart_flag = '1') then 
	  ah_dc <= "00" & ni;
	  ah_uc <= "00000000";
        elsif(ah_dc /= "00000000" and al_dc = "0001") then 
	  ah_dc <= ah_dc - "00000001";
	  ah_uc <= ah_uc + "00000001";
	end if;
      end if;
    end if;
  end process;

  address_pl <= ah_uc & al_uc;

  with ah_dc select
    vdflag_pl <= '0' when "00000000",
                 '1' when others;

-- ramdom read counter

  process(clk) begin
    if(clk'event and clk='1') then
      if(rst = '1') then  
        random_dc <= "00";
      elsif(scphase = '1') then 
        if(rdstart_flag = '1') then 
          random_uc <= com1(11 downto 0); 
          random_dc <= "10";
        elsif(random_dc /= "00") then 
          random_uc <= random_uc + "000000000001";
          random_dc <= random_dc - "01";
	end if;
      end if;
    end if;
  end process;

  address_rd <= random_uc;
 
  with random_dc select 
    vdflag_rd <= '0' when "00",
	         '1' when others;

-- address selector

  with select_code select
    f_pa <= address_rd when "10",
	    address_pl when others;

-- data selector

   fodata0 <= f_pd;

-- add parity

  process(clk) 
    variable tmp: std_logic_vector(3 downto 0);  
  begin
    if(clk'event and clk='1') then
      if(scphase = '1') then 
        tmp := "0000";
        for i in 0 to 7 loop
          tmp(0) := tmp(0) xor fodata0(i);
          tmp(1) := tmp(1) xor fodata0(i+8);
          tmp(2) := tmp(2) xor fodata0(i+16);
          tmp(3) := tmp(3) xor fodata0(i+24);
        end loop;
        f_fodata <= tmp & fodata0;
      end if;
    end if;
  end process;
 
-- output logic

  with select_code select
    vd0 <= vdflag_rd when "10",
	   vdflag_pl when others;

  process(clk) begin
    if(clk'event and clk='1') then 
      if(scphase = '1') then 
	vd1 <= vd0;
        f_vd <= not vd1;
        f_sts <= not f_csts;
        f_nd <= not vd1;
      end if;      	      
    end if;
  end process;

-- write to command register in fo

  process(clk) begin
    if(clk'event and clk='1') then 
      if(f_wefo='1') then 
        if(f_adr = "0000") then 
          com0 <= f_data;
        elsif(f_adr = "0001") then 
          com1 <= f_data;
        elsif(f_adr = "0010") then            
          ni <= f_data(5 downto 0);
        elsif(f_adr = "0011") then            
          ndata <= f_data(3 downto 0);
        end if; 
      end if;	
    end if;
  end process;

  process(clk) begin
    if(clk'event and clk='1') then 
      if(f_wefo='1' and f_adr = "0001") then 
        comstart <= '1';
      elsif(scphase = '1') then 
	comstart <= '0';	
      end if;
    end if;
  end process;
    
end RTL;
library ieee;                                                  
use ieee.std_logic_1164.all;                                   
                                                               
entity pg_pipe is                                              
  generic(JDATA_WIDTH : integer := 72);                        
  port(                                                        
    p_jdata : in std_logic_vector(JDATA_WIDTH-1 downto 0);     
    p_run : in std_logic;                                      
    p_we :  in std_logic;                                      
    p_adri : in std_logic_vector(11 downto 0);                 
    p_datai : in std_logic_vector(31 downto 0);                
    p_adro : in std_logic_vector(11 downto 0);                 
    p_datao : out std_logic_vector(31 downto 0);               
    p_runret : out std_logic;                                  
    rst,clk : in std_logic                                     
  );                                                           
end pg_pipe;                                                   
                                                               
architecture std of pg_pipe is                                 
                                                               
  component pipe                                               
    generic(JDATA_WIDTH : integer );                           
    port(                                                      
      p_jdata: in std_logic_vector(JDATA_WIDTH-1 downto 0);    
      p_run : in std_logic;                                    
      p_we : in std_logic;                                     
      p_adri : in std_logic_vector(3 downto 0);                
      p_adrivp : in std_logic_vector(3 downto 0);              
      p_datai : in std_logic_vector(31 downto 0);              
      p_adro : in std_logic_vector(3 downto 0);                
      p_adrovp : in std_logic_vector(3 downto 0);              
      p_datao : out std_logic_vector(31 downto 0);             
      p_runret : out std_logic;                                
      rst,pclk : in std_logic );                                
  end component;                                               
                                                               
  signal u_adri,u_adro: std_logic_vector(7 downto 0);          
  signal adrivp,adrovp: std_logic_vector(3 downto 0);          
  signal we,runret: std_logic_vector(0 downto 0);
  signal datao: std_logic_vector(31 downto 0);
  signal l_adro: std_logic_vector(3 downto 0);                 
                                                               
begin                                                          
                                                               
  u_adri <= p_adri(11 downto 4);                               
                                                               
  u_adro <= p_adro(11 downto 4);                               
  l_adro <= p_adro(3 downto 0);                                
                                                               
  process(u_adri,p_we) begin                                 
    if(p_we = '1') then                                      
     if(u_adri = "00000000" or u_adri = "00000001" ) then
        we(0) <= '1';                                    
      else                                                   
        we(0) <= '0';                                    
      end if;                                                
    else                                                     
      we(0) <= '0';                                      
    end if;                                                  
  end process;                                               
                                                             
  with u_adri select                                           
    adrivp <= "0000" when "00000000", 
              "0001" when "00000001", 
              "0000" when others;                            
                                                               
  with u_adro select                                           
    adrovp <= "0000" when "00000000", 
              "0001" when "00000001", 
              "0000" when others;                            
                                                               
  forgen1: for i in 0 to 0 generate           
    upipe: pipe GENERIC MAP(JDATA_WIDTH=>JDATA_WIDTH)          
	      PORT MAP(p_jdata=>p_jdata, p_run=>p_run,               
                 p_we=>we(i),p_adri=>p_adri(3 downto 0),p_adrivp=>adrivp,
	               p_datai=>p_datai,p_adro=>l_adro,p_adrovp=>adrovp, 
	               p_datao=>datao(32*(i+1)-1 downto 32*i), p_runret=>runret(i), 
		       rst=>rst,pclk=>clk);                           
  end generate forgen1;                                        
                                                               
  p_runret <= runret(0);                                       
                                                               
  with u_adro select                                           
    p_datao <= datao(31 downto 0) when "00000000", 
               datao(31 downto 0) when "00000001", 
               datao(31 downto 0) when others;                  
                                                               
end std;                                               

library ieee;                                                       
use ieee.std_logic_1164.all;                                        
use ieee.std_logic_unsigned.all;                                    
                                                                    
entity pipe is                                                      
  generic(JDATA_WIDTH : integer :=72) ;                             
port(p_jdata : in std_logic_vector(JDATA_WIDTH-1 downto 0);         
     p_run : in std_logic;                                          
     p_we :  in std_logic;                                          
     p_adri : in std_logic_vector(3 downto 0);                      
     p_adrivp : in std_logic_vector(3 downto 0);                    
     p_datai : in std_logic_vector(31 downto 0);                    
     p_adro : in std_logic_vector(3 downto 0);                      
     p_adrovp : in std_logic_vector(3 downto 0);                    
     p_datao : out std_logic_vector(31 downto 0);                   
     p_runret : out std_logic;                                      
     rst,pclk : in std_logic);                                      
end pipe;                                                           
                                                                    
architecture std of pipe is                                         
                                                                    
  component pg_fix_addsub                                          
    generic(PG_WIDTH: integer;                                     
            PG_PIPELINE: integer;                                 
            PG_DIRECTION: string);                                 
    port(x,y : in std_logic_vector(PG_WIDTH-1 downto 0);           
	 z : out std_logic_vector(PG_WIDTH-1 downto 0);              
	 clk : in std_logic);                                        
  end component;                                                   
                                                                   
  component pg_conv_ftol_32_17_8                       
    port(fixdata : in std_logic_vector(31 downto 0);     
         logdata : out std_logic_vector(16 downto 0);    
         clk : in std_logic);                                      
  end component;                                                   
                                                                   
  component pg_pdelay_8                            
    generic (PG_WIDTH: integer);                          
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);   
	     y : out std_logic_vector(PG_WIDTH-1 downto 0); 
	     clk: in std_logic);                            
  end component;                                          
                                                          
  component pg_log_shift_1                        
    generic (PG_WIDTH: integer);                           
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);    
	     y : out std_logic_vector(PG_WIDTH-1 downto 0);  
	     clk: in std_logic);                             
  end component;                                           
                                                           
  component pg_pdelay_5                            
    generic (PG_WIDTH: integer);                          
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);   
	     y : out std_logic_vector(PG_WIDTH-1 downto 0); 
	     clk: in std_logic);                            
  end component;                                          
                                                          
  component pg_log_unsigned_add_17_8                    
    port( x,y : in std_logic_vector(16 downto 0);      
          z : out std_logic_vector(16 downto 0);       
          clock : in std_logic);                                   
  end component;                                                   
                                                                   
  component pg_log_shift_m1                        
    generic (PG_WIDTH: integer);                           
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);    
	     y : out std_logic_vector(PG_WIDTH-1 downto 0);  
	     clk: in std_logic);                             
  end component;                                           
                                                           
  component pg_log_muldiv_1                             
    generic (PG_WIDTH : integer;                                 
             PG_MULDIV : string);                                
    port( x,y : in std_logic_vector(PG_WIDTH-1 downto 0);        
	     z : out std_logic_vector(PG_WIDTH-1 downto 0);        
	     clk: in std_logic);                                   
  end component;                                                 
                                                                 
  component pg_pdelay_12                            
    generic (PG_WIDTH: integer);                          
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);   
	     y : out std_logic_vector(PG_WIDTH-1 downto 0); 
	     clk: in std_logic);                            
  end component;                                          
                                                          
  component pg_conv_ltof_17_8_57      
    port(logdata : in std_logic_vector(16 downto 0);   
	     fixdata : out std_logic_vector(56 downto 0);  
	     clk : in std_logic);                                     
  end component;                                                  
                                                                  
  component pg_fix_accum_57_64_2                          
    port (fdata: in std_logic_vector(56 downto 0);    
          sdata0: out std_logic_vector(63 downto 0);   
          sdata1: out std_logic_vector(63 downto 0);   
          run: in std_logic;                                        
          clk: in std_logic);                                       
  end component;                                                    
                                                                    
  signal run: std_logic_vector(18 downto 0);                        
  signal xi: std_logic_vector(31 downto 0);    
  signal xj: std_logic_vector(31 downto 0);    
  signal yi: std_logic_vector(31 downto 0);    
  signal yj: std_logic_vector(31 downto 0);    
  signal zi: std_logic_vector(31 downto 0);    
  signal zj: std_logic_vector(31 downto 0);    
  signal xij: std_logic_vector(31 downto 0);    
  signal yij: std_logic_vector(31 downto 0);    
  signal zij: std_logic_vector(31 downto 0);    
  signal dx: std_logic_vector(16 downto 0);    
  signal dy: std_logic_vector(16 downto 0);    
  signal dz: std_logic_vector(16 downto 0);    
  signal ieps2: std_logic_vector(16 downto 0);    
  signal x2: std_logic_vector(16 downto 0);           
  signal y2: std_logic_vector(16 downto 0);           
  signal z2: std_logic_vector(16 downto 0);           
  signal ieps2r: std_logic_vector(16 downto 0);           
  signal x2y2: std_logic_vector(16 downto 0);           
  signal z2e2: std_logic_vector(16 downto 0);           
  signal r2: std_logic_vector(16 downto 0);    
  signal r1: std_logic_vector(16 downto 0);    
  signal mj: std_logic_vector(16 downto 0);    
  signal mjr: std_logic_vector(16 downto 0);    
  signal r3: std_logic_vector(16 downto 0);    
  signal mf: std_logic_vector(16 downto 0);    
  signal dxr: std_logic_vector(16 downto 0);    
  signal dyr: std_logic_vector(16 downto 0);    
  signal dzr: std_logic_vector(16 downto 0);    
  signal fx: std_logic_vector(16 downto 0);    
  signal fy: std_logic_vector(16 downto 0);    
  signal fz: std_logic_vector(16 downto 0);    
  signal ffx: std_logic_vector(56 downto 0);    
  signal sx0: std_logic_vector(63 downto 0);    
  signal sx1: std_logic_vector(63 downto 0);    
  signal ffy: std_logic_vector(56 downto 0);    
  signal sy0: std_logic_vector(63 downto 0);    
  signal sy1: std_logic_vector(63 downto 0);    
  signal ffz: std_logic_vector(56 downto 0);    
  signal sz0: std_logic_vector(63 downto 0);    
  signal sz1: std_logic_vector(63 downto 0);    
  signal xi0: std_logic_vector(31 downto 0);
  signal xi1: std_logic_vector(31 downto 0);
  signal yi0: std_logic_vector(31 downto 0);
  signal yi1: std_logic_vector(31 downto 0);
  signal zi0: std_logic_vector(31 downto 0);
  signal zi1: std_logic_vector(31 downto 0);
  signal ieps20: std_logic_vector(16 downto 0);
  signal ieps21: std_logic_vector(16 downto 0);
  signal vmp_phase: std_logic_vector(3 downto 0);                 
  signal irun: std_logic_vector(1 downto 0);      
  signal jdata1: std_logic_vector(JDATA_WIDTH-1 downto 0);      
                                                                    
begin                                                               
                                                                    
  process(pclk) begin
    if(pclk'event and pclk='1') then
      jdata1 <= p_jdata;            
    end if;
  end process;
              
  process(pclk) begin
    if(pclk'event and pclk='1') then
      if(vmp_phase = "0000") then
        xj(31 downto 0) <= jdata1(31 downto 0);
        yj(31 downto 0) <= jdata1(63 downto 32);
        zj(31 downto 0) <= p_jdata(31 downto 0);
        mj(16 downto 0) <= p_jdata(48 downto 32);
      end if;
    end if;
  end process;

  process(pclk) begin
    if(pclk'event and pclk='1') then
      if(p_we ='1') then
        if(p_adri = "0000") then
          if(p_adrivp = "0000") then
            xi0 <=  p_datai(31 downto 0);
          elsif(p_adrivp = "0001") then
            xi1 <=  p_datai(31 downto 0);
          end if;
        elsif(p_adri = "0001") then
          if(p_adrivp = "0000") then
            yi0 <=  p_datai(31 downto 0);
          elsif(p_adrivp = "0001") then
            yi1 <=  p_datai(31 downto 0);
          end if;
        elsif(p_adri = "0010") then
          if(p_adrivp = "0000") then
            zi0 <=  p_datai(31 downto 0);
          elsif(p_adrivp = "0001") then
            zi1 <=  p_datai(31 downto 0);
          end if;
        elsif(p_adri = "0011") then
          if(p_adrivp = "0000") then
            ieps20 <=  p_datai(16 downto 0);
          elsif(p_adrivp = "0001") then
            ieps21 <=  p_datai(16 downto 0);
          end if;
        end if;
      end if;
    end if;
  end process;

  process(pclk) begin
    if(pclk'event and pclk='1') then
      if(vmp_phase = "0000") then
        xi <= xi0;
        yi <= yi0;
        zi <= zi0;
        ieps2 <= ieps20;
      elsif(vmp_phase = "0001") then
        xi <= xi1;
        yi <= yi1;
        zi <= zi1;
        ieps2 <= ieps21;
      end if;
    end if;
  end process;

  process(pclk) begin                                 
    if(pclk'event and pclk='1') then                  
      irun(0) <= p_run;                               
      irun(1) <= irun(0);                   
    end if;                                           
  end process;                                        
                                                      
  process(pclk) begin                                 
    if(pclk'event and pclk='1') then                  
      if(irun(1) = '1') then           
        if(vmp_phase = "0001") then              
          vmp_phase <= "0000";                      
        else                                          
          vmp_phase <= vmp_phase + "0001";          
        end if;                                       
      else                                            
        if(p_run = '1') then                          
          vmp_phase <= "0000";                      
        end if;                                       
      end if;                                         
    end if;                                           
  end process;                                        
                                                      
  process(pclk) begin                                               
    if(pclk'event and pclk='1') then                                
      run(0) <= p_run;                                              
      for i in 0 to 17 loop                                         
        run(i+1) <= run(i);                                         
      end loop;                                                     
      p_runret <= run(18);                                          
    end if;                                                         
  end process;                                                      
                                                                    
  u0: pg_fix_addsub generic map (PG_WIDTH=>32,PG_PIPELINE=>1,     
                              PG_DIRECTION=>"SUB")                
               port map (x=>xi,y=>xj,z=>xij,clk=>pclk);             
                                                                   
  u1: pg_fix_addsub generic map (PG_WIDTH=>32,PG_PIPELINE=>1,     
                              PG_DIRECTION=>"SUB")                
               port map (x=>yi,y=>yj,z=>yij,clk=>pclk);             
                                                                   
  u2: pg_fix_addsub generic map (PG_WIDTH=>32,PG_PIPELINE=>1,     
                              PG_DIRECTION=>"SUB")                
               port map (x=>zi,y=>zj,z=>zij,clk=>pclk);             
                                                                   
  u3: pg_conv_ftol_32_17_8   port map (fixdata=>xij,logdata=>dx,clk=>pclk);                 
                                                                     
  u4: pg_conv_ftol_32_17_8   port map (fixdata=>yij,logdata=>dy,clk=>pclk);                 
                                                                     
  u5: pg_conv_ftol_32_17_8   port map (fixdata=>zij,logdata=>dz,clk=>pclk);                 
                                                                     
  u6: pg_pdelay_8 generic map(PG_WIDTH=>17)  
                    port map(x=>dx,y=>dxr,clk=>pclk); 
                                                          
  u7: pg_pdelay_8 generic map(PG_WIDTH=>17)  
                    port map(x=>dy,y=>dyr,clk=>pclk); 
                                                          
  u8: pg_pdelay_8 generic map(PG_WIDTH=>17)  
                    port map(x=>dz,y=>dzr,clk=>pclk); 
                                                          
  u9: pg_log_shift_1 generic map(PG_WIDTH=>17)       
                    port map(x=>dx,y=>x2,clk=>pclk);         
                                                                  
  u10: pg_log_shift_1 generic map(PG_WIDTH=>17)       
                    port map(x=>dy,y=>y2,clk=>pclk);         
                                                                  
  u11: pg_log_shift_1 generic map(PG_WIDTH=>17)       
                    port map(x=>dz,y=>z2,clk=>pclk);         
                                                                  
  u12: pg_pdelay_5 generic map(PG_WIDTH=>17)  
                    port map(x=>ieps2,y=>ieps2r,clk=>pclk); 
                                                          
  u13: pg_log_unsigned_add_17_8      
            port map(x=>x2,y=>y2,z=>x2y2,clock=>pclk);       
                                                                   
  u14: pg_log_unsigned_add_17_8      
            port map(x=>z2,y=>ieps2r,z=>z2e2,clock=>pclk);       
                                                                   
  u15: pg_log_unsigned_add_17_8      
            port map(x=>x2y2,y=>z2e2,z=>r2,clock=>pclk);       
                                                                   
  u16: pg_log_shift_m1 generic map(PG_WIDTH=>17)       
                    port map(x=>r2,y=>r1,clk=>pclk);         
                                                                  
  u17: pg_log_muldiv_1 generic map(PG_WIDTH=>17,
                                PG_MULDIV=>"ADD")
                    port map(x=>r2,y=>r1,z=>r3,clk=>pclk);
                                                                  
  u18: pg_pdelay_12 generic map(PG_WIDTH=>17)  
                    port map(x=>mj,y=>mjr,clk=>pclk); 
                                                          
  u19: pg_log_muldiv_1 generic map(PG_WIDTH=>17,
                                PG_MULDIV=>"SUB")
                    port map(x=>mjr,y=>r3,z=>mf,clk=>pclk);
                                                                  
  u20: pg_log_muldiv_1 generic map(PG_WIDTH=>17,
                                PG_MULDIV=>"ADD")
                    port map(x=>mf,y=>dxr,z=>fx,clk=>pclk);
                                                                  
  u21: pg_log_muldiv_1 generic map(PG_WIDTH=>17,
                                PG_MULDIV=>"ADD")
                    port map(x=>mf,y=>dyr,z=>fy,clk=>pclk);
                                                                  
  u22: pg_log_muldiv_1 generic map(PG_WIDTH=>17,
                                PG_MULDIV=>"ADD")
                    port map(x=>mf,y=>dzr,z=>fz,clk=>pclk);
                                                                  
  u23: pg_conv_ltof_17_8_57
                  port map (logdata=>fx,fixdata=>ffx,clk=>pclk);
                                                                   
  u24: pg_conv_ltof_17_8_57
                  port map (logdata=>fy,fixdata=>ffy,clk=>pclk);
                                                                   
  u25: pg_conv_ltof_17_8_57
                  port map (logdata=>fz,fixdata=>ffz,clk=>pclk);
                                                                   
  u26: pg_fix_accum_57_64_2
                   port map(fdata=>ffx,sdata0=>sx0,sdata1=>sx1,run=>run(17),clk=>pclk);
                                                                    
  u27: pg_fix_accum_57_64_2
                   port map(fdata=>ffy,sdata0=>sy0,sdata1=>sy1,run=>run(17),clk=>pclk);
                                                                    
  u28: pg_fix_accum_57_64_2
                   port map(fdata=>ffz,sdata0=>sz0,sdata1=>sz1,run=>run(17),clk=>pclk);
                                                                    
  process(pclk) begin
    if(pclk'event and pclk='1') then
      if(p_adro = "0000") then
        if(p_adrovp = "0000") then
          p_datao <= sx0(31 downto 0);
        elsif(p_adrovp = "0001") then
          p_datao <= sx1(31 downto 0);
        end if;
      elsif(p_adro = "0001") then
        if(p_adrovp = "0000") then
          p_datao <= sx0(63 downto 32);
        elsif(p_adrovp = "0001") then
          p_datao <= sx1(63 downto 32);
        end if;
      elsif(p_adro = "0010") then
        if(p_adrovp = "0000") then
          p_datao <= sy0(31 downto 0);
        elsif(p_adrovp = "0001") then
          p_datao <= sy1(31 downto 0);
        end if;
      elsif(p_adro = "0011") then
        if(p_adrovp = "0000") then
          p_datao <= sy0(63 downto 32);
        elsif(p_adrovp = "0001") then
          p_datao <= sy1(63 downto 32);
        end if;
      elsif(p_adro = "0100") then
        if(p_adrovp = "0000") then
          p_datao <= sz0(31 downto 0);
        elsif(p_adrovp = "0001") then
          p_datao <= sz1(31 downto 0);
        end if;
      elsif(p_adro = "0101") then
        if(p_adrovp = "0000") then
          p_datao <= sz0(63 downto 32);
        elsif(p_adrovp = "0001") then
          p_datao <= sz1(63 downto 32);
        end if;
      else
        p_datao <= "00000000000000000000000000000000";
      end if;
    end if;
  end process;

end std;                                                            
                                                                    
                                                                   
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
                                                                   
entity pg_conv_ftol_32_17_8 is         
  port(fixdata : in std_logic_vector(31 downto 0);      
       logdata : out std_logic_vector(16 downto 0);     
       clk : in std_logic);                                        
end pg_conv_ftol_32_17_8;              
                                                                   
architecture rtl of pg_conv_ftol_32_17_8 is 
                                                                   
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
                                                                 
  u4: table generic map (IN_WIDTH=>10,OUT_WIDTH=>8,TABLE_FILE=>"ftl0.mif") 
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
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_log_unsigned_add_17_8 is            
  port( x,y : in std_logic_vector(16 downto 0);        
        z : out std_logic_vector(16 downto 0);         
        clock : in std_logic);                                     
end pg_log_unsigned_add_17_8;                 
                                                                   
architecture rtl of pg_log_unsigned_add_17_8 is
                                                                   
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
  signal x2,x3,x4 : std_logic_vector(15 downto 0);     
  signal d0,d1,d4 : std_logic_vector(15 downto 0);     
  signal df0,df1 : std_logic;                                      
  signal z0 : std_logic_vector(15 downto 0);           
  signal d2 : std_logic_vector(8 downto 0);          
  signal sign0,sign1,sign2 : std_logic;                            
  signal signxy : std_logic_vector(1 downto 0);                    
                                                                   
begin                                                              
                                                                   
  x1 <= '0' & x(15 downto 0);                          
  y1 <= '0' & y(15 downto 0);                          
  u1: lpm_add_sub generic map(LPM_WIDTH=>17,LPM_DIRECTION=>"SUB")
       port map(dataa=>x1,datab=>y1,result=>xy);                   
  u2: lpm_add_sub generic map(LPM_WIDTH=>17,LPM_DIRECTION=>"SUB")
       port map(dataa=>y1,datab=>x1,result=>yx);                   
                                                                   
  x2 <= x(15 downto 0) when yx(16)='1' else y(15 downto 0);
  d0 <= xy(15 downto 0) when yx(16)='1' else yx(15 downto 0);
                                                                   
  signxy <= x(16)&y(16);                    
  with signxy select                                               
    sign0 <= y(16) when "01",                        
             x(16) when others;                        
                                                                   
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      x3 <= x2;                                                    
      d1 <= d0;                                                    
      sign1 <= sign0;                                              
    end if;                                                        
  end process;                                                     
                                                                   
  df0 <= '1' when d1(15 downto 12)="0000" else '0';
  u3: table generic map(IN_WIDTH=>12,OUT_WIDTH=>9,TABLE_FILE=>"add0.mif")
            port map(indata=>d1(11 downto 0),outdata=>d2,clk=>clock);
                                                                   
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      df1 <= df0;                                                  
      x4 <= x3;                                                    
      sign2 <= sign1;                                              
    end if;                                                        
  end process;                                                     
                                                                   
  d4(8 downto 0) <= d2 when (df1 = '1') else "000000000";
  d4(15 downto 9) <= "0000000";           
                                                                   
  u4: lpm_add_sub generic map(LPM_WIDTH=>16,LPM_DIRECTION=>"ADD")
       port map(dataa=>x4,datab=>d4,result=>z0);                   
                                                                   
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      z(15 downto 0) <= z0;                            
      z(16) <= sign2;                                  
    end if;                                                        
  end process;                                                     
                                                                   
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
                                                                   
entity pg_conv_ltof_17_8_57 is        
  port(logdata : in std_logic_vector(16 downto 0);
       fixdata : out std_logic_vector(56 downto 0);
       clk : in std_logic);                                        
end pg_conv_ltof_17_8_57;             
                                                                   
architecture rtl of pg_conv_ltof_17_8_57 is 
                                                                   
  component table                                                  
    generic (IN_WIDTH: integer ;                                   
             OUT_WIDTH: integer ;                                  
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
                                                                   
  u3: shift_ltof_9_56                       
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
        if(count = "0001") then                               
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
        if(count2 = "0001") then                              
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
  with count select                                             
    sxl2 <= sxl20 when "0000",                                
            sxl21 when "0001",                       
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
                                                                  
  with count2 select                                             
    sxh2 <= sxh20 when "0000",                                 
            sxh21 when "0001",                       
            sxh20 when others;                                   
                                                                    
end rtl;                                                            
